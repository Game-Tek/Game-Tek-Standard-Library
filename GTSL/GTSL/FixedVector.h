#pragma once

#include "Core.h"
#include "Bitman.h"
#include "Memory.h"
#include "Range.h"
#include <new>

#include "Algorithm.h"
#include "BitTracker.h"
#include "Result.h"

namespace GTSL
{
	/**
	 * \brief A vector that maintains indices for placed objects during their lifetime.
	 * \tparam T Type of the object this KeepVector will store.
	 */
	template<typename T, class ALLOCATOR>
	class FixedVector
	{
	public:
		using length_type = uint32;

		FixedVector() = default;

		explicit FixedVector(const length_type min, const ALLOCATOR& allocatorReference) : allocator(allocatorReference) {
			data = allocate(min, &capacity);
			InitializeBits(getIndices());
		}

		void Initialize(const length_type min, const ALLOCATOR& allocatorReference) {
			allocator = allocatorReference;
			data = allocate(min, &capacity);
			InitializeBits(getIndices());
		}

		~FixedVector() {
			if (data) [[likely]] {
				Clear();
				free();
			}
		}

		template<typename T>
		struct FixedVectorIterator
		{
			//begin
			FixedVectorIterator(FixedVector* keepVector) : keepVector(keepVector) {}

			bool operator!=(const bool other) {
				return pos != keepVector->capacity;
			}

			FixedVectorIterator& operator++() {
				do {
					++pos;
				} while (CheckBit(modulo(pos), ~keepVector->getIndices()[pos / BITS]) && pos < keepVector->capacity);

				return *this;
			}

			T& operator*() { return keepVector->getObjects()[pos]; }
			T* operator->() { return keepVector->getObjects().begin() + pos; }

		private:
			uint32 pos = 0;
			FixedVector<T, ALLOCATOR>* keepVector = nullptr;

			static constexpr uint32 modulo(const uint32 key) { return key & (BITS - 1); }
		};
		
		FixedVectorIterator<T> begin() {
			return FixedVectorIterator<T>(this);
		}

		FixedVectorIterator<const T> begin() const {
			return FixedVectorIterator<const T>(this);
		}

		[[nodiscard]] bool end() const { return true; }

		/**
		 * \brief Emplaces an object into the vector At the first free slot available.
		 * \param args Arguments for construction of object of type T.
		 * \return Index At which it was emplaced.
		 */
		template <typename... ARGS>
		length_type Emplace(ARGS&&... args) {
			auto res = OccupyFirstFreeSlot(getIndices(), capacity);
			
			if (!res) { //If there is a free index insert there,
				resize();
				res = OccupyFirstFreeSlot(getIndices(), capacity);
			}

			new(getObjects().begin() + res.Get()) T(GTSL::ForwardRef<ARGS>(args)...);

			return res.Get();
		}

		/**
		 * \brief Emplaces an object into the vector At the first free slot available.
		 * \param args Arguments for construction of object of type T.
		 * \return Index At which it was emplaced.
		 */
		template <typename... ARGS>
		void EmplaceAt(length_type index, ARGS&&... args) {
			if(index < capacity) { resize(index + 1); }
			SetAsOccupied(getIndices(), index);
			new(getObjects().begin() + index) T(GTSL::ForwardRef<ARGS>(args)...);
		}

		template<class ALLOC>
		void Copy(const FixedVector<T, ALLOC>& other) {
			if (this->capacity < other.capacity) { resize(other.capacity); }

			for (uint32 num = 0, num32 = 0; num < other.getIndices().ElementCount(); ++num, num32 += BITS) {
				for (uint32 i = 0; i < BITS; ++i) {
					if (other.IsSlotOccupied(num32 + i)) { getObjects()[num32 + i] = other.getObjects()[num32 + i]; }
				}

				getIndices()[num] = other.getIndices()[num];
			}
		}

		//[[nodiscard]] Result<uint32> GetFirstFreeIndex() const
		//{
		//	uint32 elementIndex = 0;
		//	for (auto& e : getIndices()) {
		//		if (auto bit = FindFirstClearBit(e); bit.State()) { return Result(elementIndex + bit.Get(), true); }
		//		elementIndex += BITS;
		//	}
		//
		//	return Result(MoveRef(elementIndex), false);
		//}

		/**
		 * \brief Destroys the object At the specified index which makes space for another object to take it's Place.
		 * \param index Index of the object to remove.
		 */
		void Pop(const length_type index) {
			GTSL::Destroy(getObjects().begin()[index]);
			GTSL::SetAsFree(getIndices(), index);
		}

		[[nodiscard]] bool IsSlotOccupied(const uint32 index) const {
			return GTSL::IsSlotOccupied(getIndices(), index);
		}

		void Clear() {
			for (uint32 num = 0, num32 = 0; num < getIndices().ElementCount(); ++num, num32 += BITS) {
				for (uint32 i = 0; i < BITS; ++i) {
					if (IsSlotOccupied(num32 + i)) { Pop(num32 + i); }
				}
			}
		}

		[[nodiscard]] uint32 GetCapacity() const { return capacity; }

		T& operator[](const uint32 i) {
			if constexpr (_DEBUG) {
				const auto number = i / BITS; const auto bit = i % BITS;
				auto c = getIndices()[number];
				GTSL_ASSERT(CheckBit(bit, getIndices()[number]), "No valid value in that slot!")
			}
			return getObjects()[i];
		}

		const T& operator[](const uint32 i) const {
			if constexpr (_DEBUG) {
				const auto number = i / BITS; const auto bit = i % BITS;
				auto c = getIndices()[number];
				GTSL_ASSERT(CheckBit(bit, getIndices()[number]), "No valid value in that slot!")
			}
			return getObjects()[i];
		}

		using type = T;

	private:
		friend struct FixedVectorIterator<T>;

		friend class FixedVector;

		static constexpr uint64 BITS = GTSL::Bits<length_type>();

		ALLOCATOR allocator;
		uint32 capacity = 0;
		byte* data = nullptr;

		static constexpr uint32 modulo(const length_type key) { return key & (BITS - 1); }

		byte* allocate(const uint32 newElementCount, uint32* newCapacity) {
			auto totalSize = getDataSize(newElementCount) + getIndicesSize(newElementCount);
			uint64 allocatedSize; void* newAlloc;
			allocator.Allocate(totalSize, alignof(T), &newAlloc, &allocatedSize);
			*newCapacity = newElementCount;
			return static_cast<byte*>(newAlloc);
		}

		void resize() {
			resize(capacity * 2);
		}
		
		void resize(const uint32 newSize)
		{
			uint32 newCapacity;
			auto* newAlloc = allocate(newSize, &newCapacity);

			GTSL::MemCopy(getDataSize(capacity), getObjects().begin(), newAlloc);
			GTSL::MemCopy(getIndicesSize(capacity), getIndices().begin(), newAlloc + getDataSize(newCapacity));

			free();
			data = newAlloc;
			auto oldCapacity = capacity;
			capacity = newCapacity;

			for (uint32 i = (oldCapacity / BITS) + 1; i < (capacity / BITS) + 1; ++i) {
				getIndices()[i] = 0;
			}
		}

		void free() {
			auto totalSize = getDataSize(capacity) + getIndicesSize(capacity);
			allocator.Deallocate(totalSize, alignof(T), data);
			data = nullptr;
		}
		
		[[nodiscard]] uint32 getDataSize(const uint32 newElementCount) const { return sizeof(T) * newElementCount; }
		[[nodiscard]] uint32 getIndicesSize(const uint32 newElementCount) const { return ((newElementCount / BITS) + 1) * sizeof(uint32); }

		[[nodiscard]] Range<uint32*> getIndices() { return GTSL::Range<uint32*>((capacity / BITS) + 1, reinterpret_cast<uint32*>(data + getDataSize(capacity))); }
		[[nodiscard]] Range<const uint32*> getIndices() const { return GTSL::Range<uint32*>((capacity / BITS) + 1, reinterpret_cast<uint32*>(data + getDataSize(capacity))); }

		[[nodiscard]] Range<T*> getObjects() { return GTSL::Range<T*>(capacity, reinterpret_cast<T*>(data)); }
		[[nodiscard]] Range<const T*> getObjects() const { return GTSL::Range<T*>(capacity, reinterpret_cast<T*>(data)); }

		template<typename TT, class ALLOC, typename L>
		friend void ForEach(FixedVector<TT, ALLOC>& keepVector, L&& lambda);

		template<typename TT, class ALLOC, typename L>
		friend void IndexedForEach(FixedVector<TT, ALLOC>& keepVector, L&& lambda);

		template<typename TT, class ALLOC, typename L>
		friend void IndexedForEach(const FixedVector<TT, ALLOC>& keepVector, L&& lambda);

		template<typename TT, class ALLOC, typename L>
		friend void ReverseForEach(FixedVector<TT, ALLOC>& keepVector, L&& lambda);
	};

	template<typename T, class ALLOCATOR, typename L>
	void ForEach(FixedVector<T, ALLOCATOR>& keepVector, L&& lambda) {
		uint32 num = 0;
		for (auto& index : keepVector.getIndices()) {
			for (uint32 i = 0; i < 32; ++i) {
				if (GTSL::CheckBit(i, index)) { lambda(keepVector.getObjects()[num + i]); }
			}

			num += 32;
		}
	}

	template<typename T, class ALLOCATOR, typename L>
	void IndexedForEach(FixedVector<T, ALLOCATOR>& keepVector, L&& lambda)
	{
		uint32 num = 0;
		for (auto& index : keepVector.getIndices()) {
			for (uint32 i = 0; i < 32; ++i) {
				if (GTSL::CheckBit(i, index)) { lambda(num + i, keepVector.getObjects()[num + i]); }
			}

			num += 32;
		}
	}

	template<typename T, class ALLOCATOR, typename L>
	void IndexedForEach(const FixedVector<T, ALLOCATOR>& keepVector, L&& lambda)
	{
		uint32 num = 0;
		for (auto& index : keepVector.getIndices()) {
			for (uint32 i = 0; i < 32; ++i) {
				if (GTSL::CheckBit(i, index)) { lambda(num + i, keepVector.getObjects()[num + i]); }
			}

			num += 32;
		}
	}

	template<typename T, class ALLOCATOR, typename L>
	void ReverseForEach(FixedVector<T, ALLOCATOR>& keepVector, L&& lambda)
	{
		for (int64 num = keepVector.getIndices().ElementCount() - 1, numBits = num * keepVector.BITS; num > -1; --num, numBits -= keepVector.BITS) {
			for (int64 i = 31; i > -1; --i) {
				if (keepVector.IsSlotOccupied(numBits + i)) { lambda(keepVector.getObjects()[numBits + i]); }
			}
		}
	}
}
