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
		KeepVectorIterator(const uint32 capacity, byte* dt) : capacity(capacity), data(dt)
		{
			pos = capacity - 1;
			while (GTSL::CheckBit(pos % 32, getIndices()[pos / 32])) { --pos; }
			++pos;
		}

		KeepVectorIterator(const uint32 p, const uint32 capacity, byte* dt) : pos(p), capacity(capacity), data(dt)
		{
			lastPos = capacity - 1;
			while (GTSL::CheckBit(lastPos % 32, getIndices()[lastPos / 32])) { --lastPos; }
			++lastPos;
		}

		bool operator!=(const KeepVectorIterator& other) const { return pos != other.pos; }
		bool operator==(const KeepVectorIterator& other) const { return pos == other.pos; }

		KeepVectorIterator& operator++()
		{
			//while pos is not occupied
			do
			{
				++pos;
			} while (GTSL::CheckBit(pos % 32, getIndices()[pos / 32]) && pos < lastPos);

			return *this;
		}

		KeepVectorIterator& operator--()
		{
			//while pos is not occupied
			do
			{
				--pos;
			} while (GTSL::CheckBit(pos % 32, getIndices()[pos / 32]));

			return *this;
		}

		T& operator*() { return getObjects()[pos]; }
		T* operator->() { return getObjects() + pos; }

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
			data = allocateAndSetCapacity(min);
			for (auto& e : getIndices()) { e = 0xFFFFFFFF; }
		}

		void Initialize(const length_type min, const ALLOCATOR& allocatorReference)
		{
			allocator = allocatorReference;
			data = allocateAndSetCapacity(min); for (auto& e : getIndices()) { e = 0xFFFFFFFF; }
		}

		~KeepVector()
		{			
			if(data) [[likely]]
			{
				uint32 num = 0;
				for (auto& index : getIndices())
				{
					for (uint32 i = 0; i < 32; ++i)
					{
						if (!GTSL::CheckBit(i, index)) { (getObjects().begin() + num + i)->~T(); }
					}

					num += 32;
				}
				
				free();
			}
		}
		
		KeepVectorIterator<T> begin() { return KeepVectorIterator<T>(0, capacity, data); }
		KeepVectorIterator<T> end() { return KeepVectorIterator<T>(capacity, data); }

		KeepVectorIterator<T> begin() const { return KeepVectorIterator<T>(0, capacity, data); }
		KeepVectorIterator<T> end() const { return KeepVectorIterator<T>(capacity, data); }

		Range<KeepVectorIterator<T>> GetRange() { return Range<KeepVectorIterator<T>>(begin(), end()); }
		Range<KeepVectorIterator<T>> GetRange() const { return Range<KeepVectorIterator<T>>(begin(), end()); }
		
		/**
		 * \brief Emplaces an object into the vector At the first free slot available.
		 * \param args Arguments for construction of object of type T.
		 * \return Index At which it was emplaced.
		 */
		template <typename... ARGS>
		length_type Emplace(ARGS&&... args)
		{
			length_type index = 0;

			if (findFreeIndexAndTryFlag(index)) //If there is a free index insert there,
			{
				new(getObjects().begin() + index) T(GTSL::ForwardRef<ARGS>(args)...);
				return index;
			}

			const auto oldCapacity = resize();

			GTSL::ClearBit(0, *(getIndices().begin() + oldCapacity));
			new(getObjects().begin() + index) T(GTSL::ForwardRef<ARGS>(args)...);
			
			return oldCapacity;
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
				const auto number = index / 32; const auto bit = index % 32;
				auto c = getIndices()[number];
				GTSL_ASSERT(CheckBit(bit, getIndices()[number]), "Slot is ocuppied!")
			}
			GTSL::ClearBit(index % 32, *(getIndices().begin() + index / 32));
			new(getObjects().begin() + index) T(GTSL::ForwardRef<ARGS>(args)...);
		}

		template<class ALLOC>
		void Copy(const KeepVector<T, ALLOC>& other)
		{			
			if (this->capacity < other.capacity) { resize(); }
			
			uint32 num = 0;
			for (auto& index : other.getIndices())
			{
				for (uint32 i = 0; i < 32; ++i)
				{
					if (!GTSL::CheckBit(i, index)) { getObjects()[num + i] = other[num + i]; }
				}

				getIndices()[num / 32] = index;
				num += 32;
			}
		}

		[[nodiscard]] Result<uint32> GetFirstFreeIndex() const
		{
			uint32 elementIndex = 0;
			for (auto& e : getIndices())
			{
				uint8 bit; bool anySet;
				GTSL::FindFirstSetBit(e, bit, anySet);

				if (anySet)	{ return Result<uint32>(elementIndex + bit, true); }

				elementIndex += 32;
			}

			return Result<uint32>(MoveRef(elementIndex), false);
		}
		
		/**
		 * \brief Destroys the object At the specified index which makes space for another object to take it's Place.
		 * \param index Index of the object to remove.
		 */
		void Pop(const length_type index)
		{
			const auto number = index / 32; const auto bit = index % 32;
			GTSL::SetBit(bit, *(getIndices().begin() + number));
			(getObjects().begin() + index)->~T();
		}

		[[nodiscard]] bool IsSlotOccupied(const uint32 index) const
		{
			const auto number = index / 32; const auto bit = index % 32;
			return !CheckBit(bit, number);
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
		
		ALLOCATOR allocator;
		uint32 capacity;
		byte* data;

		byte* allocateAndSetCapacity(const uint32 newElementCount)
		{
			auto totalSize = getDataSize(newElementCount) + getIndicesSize(newElementCount);
			uint64 allocatedSize; void* newAlloc;
			allocator.Allocate(totalSize, alignof(T), &newAlloc, &allocatedSize);
			capacity = newElementCount;
			return static_cast<byte*>(newAlloc);
		}

		uint32 resize()
		{
			const auto oldCapacity = capacity;
			auto* newAlloc = allocateAndSetCapacity(capacity * 2);
			GTSL::MemCopy(getDataSize(oldCapacity) + getIndicesSize(oldCapacity), data, newAlloc);
			free();
			data = newAlloc;
			for(auto* begin = getIndices().begin() + oldCapacity; begin != getIndices().begin() + capacity; ++begin) { *begin = 0xFFFFFFFF; }
			return oldCapacity;
		}
		
		void free()
		{
			auto totalSize = getDataSize(capacity) + getIndicesSize(capacity);
			allocator.Deallocate(totalSize, alignof(T), data);
		}

		[[nodiscard]] uint32 getDataSize(const uint32 newElementCount) const { return sizeof(T) * newElementCount; }
		[[nodiscard]] uint32 getIndicesSize(const uint32 newElementCount) const { return ((newElementCount / 32) + 1) * sizeof(uint32); }

		[[nodiscard]] Range<uint32*> getIndices() { return GTSL::Range<uint32*>((capacity / 32) + 1, reinterpret_cast<uint32*>(data + getDataSize(capacity))); }
		[[nodiscard]] Range<const uint32*> getIndices() const { return GTSL::Range<uint32*>((capacity / 32) + 1, reinterpret_cast<uint32*>(data + getDataSize(capacity))); }

		[[nodiscard]] Range<T*> getObjects() { return GTSL::Range<T*>(capacity, reinterpret_cast<T*>(data)); }
		[[nodiscard]] Range<const T*> getObjects() const { return GTSL::Range<T*>(capacity, reinterpret_cast<T*>(data)); }
		
		/**
		 * \brief Tries to Find a free index.
		 * \param index reference to a length_type variable to store the free index.
		 * \return A bool indicating whether or not a free index was found.
		 */
		bool findFreeIndexAndTryFlag(length_type& index)
		{
			uint32 elementIndex = 0;
			for(auto& e : getIndices())
			{
				uint8 bit; bool anySet;
				GTSL::FindFirstSetBit(e, bit, anySet);
				
				if (anySet)
				{
					GTSL::ClearBit(bit, e);
					index = elementIndex + bit; return true;
				}
				
				elementIndex += 32;
			}

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
		for(auto& index : keepVector.getIndices())
		{
			for(uint32 i = 0; i < 32; ++i)
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
		for(auto& index : keepVector.getIndices())
		{
			for(uint32 i = 0; i < 32; ++i)
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
		
		for(auto* end = keepVector.getIndices().end() - 1; end != (keepVector.getIndices().begin() - 1); --end)
		{
			for(int64 i = 31; i > -1; --i)
			{
				if (!GTSL::CheckBit(i, *end)) { lambda(keepVector.getObjects().operator[](num + i)); }
			}
			
			num -= 32;
		}
	}
}
