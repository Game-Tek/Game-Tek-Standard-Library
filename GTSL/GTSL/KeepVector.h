#pragma once

#include "Core.h"
#include "Bitman.h"
#include "Memory.h"
#include "Ranger.h"
#include <new>

namespace GTSL
{
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

		~KeepVector()
		{			
			if(data) [[likely]]
			{
				uint32 num = 0;
				for (auto& index : getIndices())
				{
					for (uint32 i = 0; i < 32; ++i)
					{
						if (!GTSL::CheckBit(i, index)) { (getObjects() + num + i)->~T(); }
					}

					num += 32;
				}
				
				free();
			}
		}
		
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
				new(getObjects() + index) T(GTSL::ForwardRef<ARGS>(args)...);
				return index;
			}

			const auto oldCapacity = resize();

			GTSL::ClearBit(0, *(getIndices() + oldCapacity));
			new(getObjects() + index) T(GTSL::ForwardRef<ARGS>(args)...);
			
			return oldCapacity;
		}

		/**
		 * \brief Destroys the object At the specified index which makes space for another object to take it's Place.
		 * \param index Index of the object to remove.
		 */
		void Pop(const length_type index)
		{
			const auto number = index / 32; const auto bit = index % 32;
			GTSL::SetBit(bit, *(getIndices() + number));
			(getObjects() + index)->~T();
		}

		void Clear()
		{
			uint32 num = 0;
			for (auto& index : getIndices())
			{
				for (uint32 i = 0; i < 32; ++i)
				{
					if (!GTSL::CheckBit(i, index)) { (getObjects() + num + i)->~T(); }
				}

				num += 32;
				index = 0xFFFFFFFF;
			}
		}
		
		[[nodiscard]] uint32 GetCapacity() const { return capacity; }
		
		const T& operator[](const uint32 i) const
		{
			const auto number = i / 32; const auto bit = i % 32;
			if constexpr (_DEBUG)
			{
				auto c = getIndices()[number];
				GTSL_ASSERT(!CheckBit(bit, getIndices()[number]), "No valid value in that slot!")
			}
			return getObjects()[number + bit];
		}
		
	private:
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
			for(auto* begin = getIndices() + oldCapacity; begin != getIndices() + capacity; ++begin) { *begin = 0xFFFFFFFF; }
			return oldCapacity;
		}
		
		void free()
		{
			auto totalSize = getDataSize(capacity) + getIndicesSize(capacity);
			allocator.Deallocate(totalSize, alignof(T), data);
		}

		[[nodiscard]] uint32 getDataSize(const uint32 newElementCount) const { return sizeof(T) * newElementCount; }
		[[nodiscard]] uint32 getIndicesSize(const uint32 newElementCount) const { return ((newElementCount / 32) + 1) * sizeof(uint32); }

		[[nodiscard]] Ranger<uint32> getIndices() { return GTSL::Ranger<uint32>((capacity / 32) + 1, reinterpret_cast<uint32*>(data + getDataSize(capacity))); }
		[[nodiscard]] Ranger<const uint32> getIndices() const { return GTSL::Ranger<uint32>((capacity / 32) + 1, reinterpret_cast<uint32*>(data + getDataSize(capacity))); }

		[[nodiscard]] Ranger<T> getObjects() { return GTSL::Ranger<T>(capacity, reinterpret_cast<T*>(data)); }
		[[nodiscard]] Ranger<const T> getObjects() const { return GTSL::Ranger<T>(capacity, reinterpret_cast<T*>(data)); }
		
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
}
