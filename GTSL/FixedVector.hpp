#pragma once

#include "Core.h"
#include "Bitman.h"
#include "Memory.h"
#include "Range.hpp"
#include "BitTracker.h"
#include "Allocator.h"
#include "Result.h"
#include <new>

namespace GTSL
{
	/**
	 * \brief A vector that maintains indices for placed objects during their lifetime.
	 * \tparam T Type of the object this KeepVector will store.
	 */
	template<typename T, class ALLOCATOR>
	class FixedVector {
	public:
		using length_type = uint32;

		FixedVector(const ALLOCATOR& allocatorReference = ALLOCATOR()) : allocator(allocatorReference) {
		}

		FixedVector(const length_type min, const ALLOCATOR& allocatorReference = ALLOCATOR()) : allocator(allocatorReference) {
			resize(min);
			InitializeBits(getIndices());
		}

		~FixedVector() {
			if (data) [[likely]] {
				Clear();
				free();
			}
		}

		template<typename T>
		struct FixedVectorIterator {
			//begin
			FixedVectorIterator(FixedVector* keepVector) : keepVector(keepVector) {}

			bool operator!=(const bool other) {
				return pos != keepVector->capacity;
			}

			FixedVectorIterator& operator++() {
				do {
					++pos;
				} while (CheckBit(modulo(pos), ~keepVector->indices[pos / BITS]) && pos < keepVector->capacity);

				return *this;
			}

			T& operator*() { return keepVector->data[pos]; }
			T* operator->() { return keepVector->data + pos; }

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
			auto res = OccupyFirstFreeSlot(getIndices());
			
			if (!res) { //If there is a free index insert there,
				resize();
				res = OccupyFirstFreeSlot(getIndices());
			}

			new(data + res.Get()) T(GTSL::ForwardRef<ARGS>(args)...);

			return res.Get();
		}

		/**
		 * \brief Emplaces an object into the vector At the first free slot available.
		 * \param args Arguments for construction of object of type T.
		 * \return Index At which it was emplaced.
		 */
		template <typename... ARGS>
		T& EmplaceAt(length_type index, ARGS&&... args) {
			if(index > capacity) { resize(index + 1); }
			SetAsOccupied(getIndices(), index);
			return *new(data + index) T(GTSL::ForwardRef<ARGS>(args)...);
		}

		/**
		 * \brief Destroys the object At the specified index which makes space for another object to take it's Place.
		 * \param index Index of the object to remove.
		 */
		void Pop(const length_type index) {
			GTSL::Destroy(data[index]);
			SetAsFree(getIndices(), index);
		}

		[[nodiscard]] bool IsSlotOccupied(const uint32 index) const {
			return GTSL::IsSlotOccupied(getIndices(), index);
		}

		void Clear() {
			for (uint32 num = 0; num < capacity; ++num) {
				if (IsSlotOccupied(num)) { Pop(num); }
			}
		}

		[[nodiscard]] uint32 GetCapacity() const { return capacity; }

		T& operator[](const uint32 i) {
#ifdef _DEBUG
				const auto number = i / BITS; const auto bit = static_cast<uint8>(i % BITS);
				auto c = indices[number];
				GTSL_ASSERT(CheckBit(bit, indices[number]), "No valid value in that slot!")
#endif
			return data[i];
		}

		const T& operator[](const uint32 i) const {
#ifdef _DEBUG
				const auto number = i / BITS; const auto bit = static_cast<uint8>(i % BITS);
				auto c = indices[number];
				GTSL_ASSERT(CheckBit(bit, indices[number]), "No valid value in that slot!")
#endif
			return data[i];
		}

		using type = T;

	private:
		friend struct FixedVectorIterator<T>;

		friend class FixedVector;

		static constexpr uint8 BITS = static_cast<uint8>(GTSL::Bits<length_type>());

		ALLOCATOR allocator;
		uint32 capacity = 0;
		T* data = nullptr;
		uint32* indices = nullptr;

		static constexpr uint32 modulo(const length_type key) { return key & (BITS - 1); }

		void resize() {
			resize(capacity * 2);
		}
		
		void resize(uint32 newSize) {
			newSize = GTSL::Math::RoundUpByPowerOf2(newSize, BITS);

			if (data) {
				Resize(allocator, &data, capacity, newSize);
				Resize(allocator, &indices, capacity / BITS, newSize / BITS);
			} else {
				Allocate(allocator, newSize, &data);
				Allocate(allocator, newSize / BITS, &indices);
			}

			for(uint32 i = capacity / BITS; i < newSize / BITS; ++i) {
				indices[i] = 0;
			}

			capacity = newSize;
		}

		void free() {
			Deallocate(allocator, capacity, data);
			Deallocate(allocator, capacity / BITS, indices);
			data = nullptr; indices = nullptr;
		}

		Range<uint32*> getIndices() { return GTSL::Range<uint32*>(capacity / BITS, indices); }
		Range<const uint32*> getIndices() const { return GTSL::Range<const uint32*>(capacity / BITS, indices); }

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
				if (keepVector.IsSlotOccupied(numBits + i)) { lambda(keepVector.data[numBits + i]); }
			}
		}
	}
}
