#pragma once

#include "Core.h"
#include "Bitman.h"
#include "Memory.h"
#include "Range.h"
#include <new>

#include "Result.h"

namespace GTSL
{
	template<typename T>
	struct KeepVectorIterator
	{
		//begin
		KeepVectorIterator(const uint32 pos, const uint32 capacity, byte* dt) : capacity(capacity), data(dt), pos(pos)
		{
		}

		bool operator!=(const KeepVectorIterator& other)
		{
			lastPos = other.pos;
			return pos != other.pos;
		}
		bool operator==(const KeepVectorIterator& other) const { return pos == other.pos; }

		KeepVectorIterator& operator++()
		{
			//while pos is not occupied
			do
			{
				++pos;
			} while (CheckBit(modulo(pos), getIndices()[pos / 32]) && pos < lastPos);

			return *this;
		}

		KeepVectorIterator& operator--()
		{
			//while pos is not occupied
			do
			{
				--pos;
			} while (CheckBit(modulo(pos), getIndices()[pos / 32]));

			return *this;
		}

		T& operator*() { return getObjects()[pos]; }
		T* operator->() { return getObjects().begin() + pos; }

		T& operator[](const uint32 index) { return getObjects()[index]; }

	private:
		uint32 pos = 0, capacity = 0, lastPos = 0;
		byte* data = nullptr;

		[[nodiscard]] uint32 getDataSize(const uint32 newElementCount) const { return sizeof(T) * newElementCount; }
		[[nodiscard]] uint32 getIndicesSize(const uint32 newElementCount) const { return ((newElementCount / 32) + 1) * sizeof(uint32); }

		[[nodiscard]] Range<uint32*> getIndices() { return GTSL::Range<uint32*>((capacity / 32) + 1, reinterpret_cast<uint32*>(data + getDataSize(capacity))); }
		[[nodiscard]] Range<const uint32*> getIndices() const { return GTSL::Range<uint32*>((capacity / 32) + 1, reinterpret_cast<uint32*>(data + getDataSize(capacity))); }

		[[nodiscard]] Range<T*> getObjects() { return GTSL::Range<T*>(capacity, reinterpret_cast<T*>(data)); }
		[[nodiscard]] Range<const T*> getObjects() const { return GTSL::Range<T*>(capacity, reinterpret_cast<T*>(data)); }

		static constexpr uint32 modulo(const uint32 key) { return key & (32 - 1); }
	};

	/**
	 * \brief A vector that maintains indices for placed objects during their lifetime.
	 * \tparam T Type of the object this KeepVector will store.
	 */
	template<typename T, class ALLOCATOR>
	class KeepVector
	{
	public:
		using length_type = uint32;

		KeepVector() = default;

		explicit KeepVector(const length_type min, const ALLOCATOR& allocatorReference) : allocator(allocatorReference)
		{
			data = allocate(min, &capacity);
			for (auto& e : getIndices()) { e = 0xFFFFFFFF; }
		}

		void Initialize(const length_type min, const ALLOCATOR& allocatorReference)
		{
			allocator = allocatorReference;
			data = allocate(min, &capacity); for (auto& e : getIndices()) { e = 0xFFFFFFFF; }
		}

		~KeepVector()
		{
			if (data) [[likely]]
			{
				uint32 num = 0;
				for (auto& index : getIndices())
				{
					for (uint32 i = 0; i < BITS; ++i)
					{
						if (!GTSL::CheckBit(i, index)) { (getObjects().begin() + num + i)->~T(); }
					}

					num += BITS;
				}

				free(capacity);
			}
		}

		KeepVectorIterator<T> begin()
		{
			uint32 start = 0, end = 0;
			getStartEnd(start, end);
			return KeepVectorIterator<T>(start, capacity, data);
		}
		
		KeepVectorIterator<T> end()
		{
			uint32 start = 0, end = 0;
			getStartEnd(start, end);
			return KeepVectorIterator<T>(end, capacity, data);
		}

		KeepVectorIterator<const T> begin() const
		{
			uint32 start = 0, end = 0;
			getStartEnd(start, end);
			return KeepVectorIterator<const T>(start, capacity, data);
		}

		KeepVectorIterator<const T> end() const
		{
			uint32 start = 0, end = 0;
			getStartEnd(start, end);
			return KeepVectorIterator<const T>(end, capacity, data);
		}

		Range<KeepVectorIterator<T>> GetRange() { return Range<KeepVectorIterator<T>>(begin(), end()); }
		Range<KeepVectorIterator<const T>> GetRange() const { return Range<KeepVectorIterator<const T>>(begin(), end()); }

		/**
		 * \brief Emplaces an object into the vector At the first free slot available.
		 * \param args Arguments for construction of object of type T.
		 * \return Index At which it was emplaced.
		 */
		template <typename... ARGS>
		length_type Emplace(ARGS&&... args)
		{
			length_type index = 0; uint8 bit = 0;

			if (!findFreeValidIndexAndTryFlag(&index, &bit)) //If there is a free index insert there,
			{
				resize();

				GTSL::ClearBit(bit, *(getIndices().begin() + index));
			}

			new(getObjects().begin() + index + bit) T(GTSL::ForwardRef<ARGS>(args)...);

			return index + bit;
		}

		/**
		 * \brief Emplaces an object into the vector At the first free slot available.
		 * \param args Arguments for construction of object of type T.
		 * \return Index At which it was emplaced.
		 */
		template <typename... ARGS>
		void EmplaceAt(length_type index, ARGS&&... args)
		{
			if (index > this->capacity) [[unlikely]] { resize(); }
				if constexpr (_DEBUG)
				{
					const auto number = index / 32; const auto bit = modulo(index);
					auto c = getIndices()[number];
					GTSL_ASSERT(CheckBit(bit, getIndices()[number]), "Slot is ocuppied!")
				}
			GTSL::ClearBit(modulo(index), *(getIndices().begin() + index / 32));
			new(getObjects().begin() + index) T(GTSL::ForwardRef<ARGS>(args)...);
		}

		template<class ALLOC>
		void Copy(const KeepVector<T, ALLOC>& other)
		{
			if (this->capacity < other.capacity) { resize(); }

			uint32 num = 0;
			for (auto& index : other.getIndices())
			{
				for (uint32 i = 0; i < BITS; ++i)
				{
					if (!GTSL::CheckBit(i, index)) { getObjects()[num + i] = other[num + i]; }
				}

				getIndices()[num / BITS] = index;
				num += BITS;
			}
		}

		[[nodiscard]] Result<uint32> GetFirstFreeIndex() const
		{
			uint32 elementIndex = 0;
			for (auto& e : getIndices())
			{
				uint8 bit; bool anySet;
				GTSL::FindFirstSetBit(e, bit, anySet);

				if (anySet) { return Result<uint32>(elementIndex + bit, true); }

				elementIndex += BITS;
			}

			return Result<uint32>(MoveRef(elementIndex), false);
		}

		/**
		 * \brief Destroys the object At the specified index which makes space for another object to take it's Place.
		 * \param index Index of the object to remove.
		 */
		void Pop(const length_type index)
		{
			const auto number = index / 32; const auto bit = modulo(index);
			GTSL::SetBit(bit, *(getIndices().begin() + number));
			(getObjects().begin() + index)->~T();
		}

		[[nodiscard]] bool IsSlotOccupied(const uint32 index) const
		{
			const auto number = index / BITS; const auto bit = modulo(index);
			return !CheckBit(bit, *(getIndices().begin() + number));
		}

		void Clear()
		{
			uint32 num = 0;
			for (auto& index : getIndices())
			{
				for (uint32 i = 0; i < 32; ++i)
				{
					if (!GTSL::CheckBit(i, index)) { (getObjects().begin() + num + i)->~T(); }
				}

				num += 32;
				index = 0xFFFFFFFF;
			}
		}

		[[nodiscard]] uint32 GetCapacity() const { return capacity; }

		T& operator[](const uint32 i)
		{
			if constexpr (_DEBUG)
			{
				const auto number = i / 32; const auto bit = i % 32;
				auto c = getIndices()[number];
				GTSL_ASSERT(!CheckBit(bit, getIndices()[number]), "No valid value in that slot!")
			}
			return getObjects()[i];
		}

		const T& operator[](const uint32 i) const
		{
			if constexpr (_DEBUG)
			{
				const auto number = i / 32; const auto bit = i % 32;
				auto c = getIndices()[number];
				GTSL_ASSERT(!CheckBit(bit, getIndices()[number]), "No valid value in that slot!")
			}
			return getObjects()[i];
		}

		using type = T;

	private:
		friend struct KeepVectorIterator<T>;

		friend class KeepVector;

		static constexpr uint64 BITS = sizeof(length_type) * 8;

		ALLOCATOR allocator;
		uint32 capacity = 0;
		byte* data = nullptr;

		static constexpr uint32 modulo(const length_type key) { return key & (BITS - 1); }

		byte* allocate(const uint32 newElementCount, uint32* newCapacity)
		{
			auto totalSize = getDataSize(newElementCount) + getIndicesSize(newElementCount);
			uint64 allocatedSize; void* newAlloc;
			allocator.Allocate(totalSize, alignof(T), &newAlloc, &allocatedSize);
			*newCapacity = newElementCount;
			return static_cast<byte*>(newAlloc);
		}

		void resize()
		{
			auto preAllocateIndices = *getIndices().begin();

			uint32 newCapacity;
			auto* newAlloc = allocate(capacity * 2, &newCapacity);

			GTSL::MemCopy(getDataSize(capacity), getObjects().begin(), newAlloc);
			GTSL::MemCopy(getIndicesSize(capacity), getIndices().begin(), newAlloc + getDataSize(newCapacity));

			free(capacity);
			data = newAlloc;

			for (auto* begin = getIndices().begin() + (capacity / BITS) + 1; begin != getIndices().begin() + (newCapacity / BITS) + 1; ++begin)
			{
				*begin = 0xFFFFFFFF;
			}

			capacity = newCapacity;
		}

		void free(const length_type size)
		{
			auto totalSize = getDataSize(size) + getIndicesSize(size);
			allocator.Deallocate(totalSize, alignof(T), data);
		}

		void getStartEnd(uint32& start, uint32& end) const
		{
			uint8 bit = 0;

			for (uint32 i = 0; i < getIndices().ElementCount(); ++i)
			{
				bool anySet;
				FindFirstSetBit(~getIndices()[i], bit, anySet); //look for first occupied slot
				if (anySet)
				{
					start = i * BITS;
					start += bit;
					break;
				}
			}
			
			for (uint32 i = 0, r = getIndices().ElementCount() - 1; i < getIndices().ElementCount(); ++i, --r)
			{
				bool anySet;
				FindLastSetBit(~getIndices()[r], bit, anySet); //look for last occupied slot
				if (anySet)
				{
					end = r * BITS;
					end += bit;
					end += 1;
					break;
				}
			}
		}
		
		[[nodiscard]] uint32 getDataSize(const uint32 newElementCount) const { return sizeof(T) * newElementCount; }
		[[nodiscard]] uint32 getIndicesSize(const uint32 newElementCount) const { return ((newElementCount / BITS) + 1) * sizeof(uint32); }

		[[nodiscard]] Range<uint32*> getIndices() { return GTSL::Range<uint32*>((capacity / BITS) + 1, reinterpret_cast<uint32*>(data + getDataSize(capacity))); }
		[[nodiscard]] Range<const uint32*> getIndices() const { return GTSL::Range<uint32*>((capacity / BITS) + 1, reinterpret_cast<uint32*>(data + getDataSize(capacity))); }

		[[nodiscard]] Range<T*> getObjects() { return GTSL::Range<T*>(capacity, reinterpret_cast<T*>(data)); }
		[[nodiscard]] Range<const T*> getObjects() const { return GTSL::Range<T*>(capacity, reinterpret_cast<T*>(data)); }

		/**
		 * \brief Tries to Find a free index.
		 * \param index reference to a length_type variable to store the free index.
		 * \return A bool indicating whether or not a free index was found.
		 */
		bool findFreeValidIndexAndTryFlag(length_type* indicesElement, uint8* bitt)
		{
			uint32 elementIndex = 0;
			for (auto& e : getIndices())
			{
				uint8 bit; bool anySet;
				GTSL::FindFirstSetBit(e, bit, anySet);

				//if a bit is set it means it's free

				if (anySet)
				{
					GTSL::ClearBit(bit, e);
					*indicesElement = elementIndex;
					*bitt = bit;

					return elementIndex + bit < capacity;
				}

				elementIndex += 32;
			}

			*indicesElement = elementIndex;
			*bitt = 0;

			return false;
		}

		template<typename TT, class ALLOC, typename L>
		friend void ForEach(KeepVector<TT, ALLOC>& keepVector, L&& lambda);

		template<typename TT, class ALLOC, typename L>
		friend void ForEachIndexed(KeepVector<TT, ALLOC>& keepVector, L&& lambda);

		template<typename TT, class ALLOC, typename L>
		friend void ReverseForEach(KeepVector<TT, ALLOC>& keepVector, L&& lambda);
	};

	template<typename T, class ALLOCATOR, typename L>
	void ForEach(KeepVector<T, ALLOCATOR>& keepVector, L&& lambda)
	{
		uint32 num = 0;
		for (auto& index : keepVector.getIndices())
		{
			for (uint32 i = 0; i < 32; ++i)
			{
				if (!GTSL::CheckBit(i, index)) { lambda(keepVector.getObjects()[num + i]); }
			}

			num += 32;
		}
	}

	template<typename T, class ALLOCATOR, typename L>
	void ForEachIndexed(KeepVector<T, ALLOCATOR>& keepVector, L&& lambda)
	{
		uint32 num = 0;
		for (auto& index : keepVector.getIndices())
		{
			for (uint32 i = 0; i < 32; ++i)
			{
				if (!GTSL::CheckBit(i, index)) { lambda(num + i, keepVector.getObjects()[num + i]); }
			}

			num += 32;
		}
	}

	template<typename T, class ALLOCATOR, typename L>
	void ReverseForEach(KeepVector<T, ALLOCATOR>& keepVector, L&& lambda)
	{
		uint32 num = (keepVector.getIndices().ElementCount() - 1) * 32;

		for (auto* end = keepVector.getIndices().end() - 1; end != (keepVector.getIndices().begin() - 1); --end)
		{
			for (int64 i = 31; i > -1; --i)
			{
				if (!GTSL::CheckBit(i, *end)) { lambda(keepVector.getObjects().operator[](num + i)); }
			}

			num -= 32;
		}
	}
}
