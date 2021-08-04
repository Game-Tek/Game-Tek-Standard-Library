#pragma once

#include "Core.h"

#include "Pair.h"
#include "Memory.h"
#include "ArrayCommon.hpp"

namespace GTSL
{
	template<typename T>
	struct Group
	{
		/**
		 * \brief Group elements array.
		 */
		T* Elements = nullptr;

		/**
		 * \brief Index of the first element of the group. Since the vector is sparse we don't know where it is and we need to keep track of it.
		 */
		uint32 First = 0;
		
		/**
		 * \brief Number of elements in this group.
		 */
		uint32 ElementCount = 0;

		uint32 Capacity = 0;

		T& operator[](const uint32 i) { return Elements[i]; }

		Pair<uint32, T&> At(const uint32 i) { return Pair<uint32, T&>(First + i, Elements[i]); }
		
		auto GetElements()  { return Range<T*>(ElementCount, Elements); }
		auto GetElements() const { return Range<const T*>(ElementCount, Elements); }
	};
	
	template<typename T>
	struct SparseVectorIterator
	{
	public:
		SparseVectorIterator(Group<T>* dt) : groups(dt)
		{
		}

		SparseVectorIterator(const uint32 groupCount, Group<T>* dt) : groups(dt), currentGroup(groupCount)
		{
		}

		bool operator!=(const SparseVectorIterator& other) const { return currentGroup != other.currentGroup; }
		bool operator==(const SparseVectorIterator& other) const { return currentGroup == other.currentGroup; }

		SparseVectorIterator& operator++()
		{
			++currentGroup; return *this;
		}

		Group<T>& operator*() { return groups[currentGroup]; }
		Group<T>* operator->() { return groups + currentGroup; }
		
	private:
		Group<T>* groups;

		uint32 currentGroup = 0;
	};
	
	template<typename T, Allocator ALLOCATOR>
	class SparseVector
	{
		static constexpr uint8 IS_PRE_CONTIGUOUS = 1;
		static constexpr uint8 IS_POST_CONTIGUOUS = 2;
		static constexpr uint8 IS_INSIDE = 4;
		
	public:
		SparseVector(const uint32 maxElements, const ALLOCATOR& allo = ALLOCATOR()) : allocator(allo)
		{
			uint64 allocatedSize;
			allocator.Allocate(sizeof(Group<T>) * maxElements, alignof(Group<T>), reinterpret_cast<void**>(&groups), &allocatedSize);
			allocatedGroups = static_cast<uint32>(allocatedSize / sizeof(Group<T>));
		}

		SparseVector(SparseVector&& other) noexcept : allocator(MoveRef(other.allocator)), groups(other.groups), allocatedGroups(other.allocatedGroups), groupCount(other.groupCount)
		{
			other.groups = nullptr;
		}
		
		~SparseVector()
		{
			if (groups)
			{
				for (uint32 i = 0; i < groupCount; ++i) {
					for(uint32 j = 0; j < groups[i].ElementCount; ++j) {
						Destroy(groups[i].Elements[j]);
					}

					allocator.Deallocate(sizeof(T) * groups[i].Capacity, alignof(T), reinterpret_cast<void*>(groups[i].Elements));
				}
				
				allocator.Deallocate(sizeof(Group<T>) * allocatedGroups, alignof(Group<T>), groups);
				groups = nullptr;
			}
		}

		template<typename... ARGS>
		T& EmplaceAt(const uint32 pos, ARGS&&... args)
		{			
			//for(uint32 i = 0; i < groupCount; ++i)
			//{				
			//	Group<T>& group = groups[i];
			//	
			//	if (pos == group.First)
			//	{
			//		GTSL_ASSERT(group.ElementCount != capacity, "No more space available");
			//		insertElement(group.GetElements(), 0);
			//		new(group.Elements) T(ForwardRef<ARGS>(args)...);
			//		group.First = pos;
			//
			//		++group.ElementCount;
			//
			//		return;
			//	}
			//	
			//	if (pos == group.First + group.ElementCount)
			//	{
			//		GTSL_ASSERT(group.ElementCount != capacity, "No more space available");
			//		new(group.Elements + group.ElementCount) T(ForwardRef<ARGS>(args)...);
			//		++group.ElementCount;
			//
			//		return;
			//	}
			//	
			//	if (pos > group.First && pos < group.First)
			//	{
			//		GTSL_ASSERT(group.ElementCount != capacity, "No more space available");
			//		insertElement(group.GetElements(), group.First - pos);
			//		new(group.Elements + group.First - pos) T(ForwardRef<ARGS>(args)...);
			//		++group.ElementCount;
			//		
			//		return;
			//	}
			//}

			{
				uint64 allocatedSize;

				GTSL_ASSERT(groupCount != allocatedGroups, "No more allocated groups!")
				
				Group<T>& group = groups[groupCount]; //TODO: GUARANTEE ORDERING
				group.First = pos;
				group.ElementCount = 1;
				group.Capacity = 4;
				allocator.Allocate(sizeof(T) * 4, alignof(T), reinterpret_cast<void**>(&group.Elements), &allocatedSize);

				auto* obj = new(group.Elements + (pos - group.First)) T(GTSL::ForwardRef<ARGS>(args)...);
				
				++groupCount;

				return *obj;
			}
		}

		void Clear()
		{
			for(uint32 i = 0; i < groupCount; ++i)
			{
				auto& group = groups[i];

				if (group.Elements)
				{
					for (uint32 j = 0; j < group.ElementCount; ++j) { Destroy(group.Elements[j]); }
					group.ElementCount = 0;
				}
			}

			groupCount = 0;
		}

		SparseVectorIterator<T> begin() { return SparseVectorIterator<T>(groups); }
		SparseVectorIterator<T> end() { return SparseVectorIterator<T>(groupCount, groups); }

		Range<SparseVectorIterator<T>> GetRange() { return Range<SparseVectorIterator<T>>(begin(), end()); }

		[[nodiscard]] uint32 GetGroupCount() const { return groupCount; }

		auto GetGroups() const { return GTSL::Range<const Group<T>*>(groupCount, groups); }

		bool IsSlotOccupied(const uint32 element) const
		{
			for (uint32 g = 0; g < groupCount; ++g)
			{
				auto& group = groups[g];
				
				if (element >= groups[g].First && element < groups[g].First + groups[g].ElementCount) //TODO: CAN REMOVE FIRST CONDITION IF WE CAN GUARANTEE AN ORDERED ARRAY
				{
					return true;
				}
			}

			return false;
		}
		
		T& operator[](const uint32 element)
		{
			for(uint32 g = 0; g < groupCount; ++g)
			{
				if(element >= groups[g].First && element < groups[g].First + groups[g].ElementCount) //TODO: CAN REMOVE FIRST CONDITION IF WE CAN GUARANTEE AN ORDERED ARRAY
				{
					return groups[g].Elements[element - groups[g].First];
				}
			}
			
			GTSL_ASSERT(false, "No such element")
			return groups[0].Elements[0];
		}

		ALLOCATOR& GetAllocator() { return allocator; }
		const ALLOCATOR& GetAllocator() const { return allocator; }
	
	private:
		ALLOCATOR allocator;
		Group<T>* groups = nullptr;

		friend struct SparseVectorIterator<T>;
		friend class SparseVector;
		
		uint32 allocatedGroups = 0;
		uint32 groupCount = 0;

		template<typename TT, class ALLOC, typename L>
		friend void ForEach(const SparseVector<TT, ALLOC>& keepVector, L&& lambda);
	};

	template<typename TT, class ALLOC, typename L>
	void ForEach(const SparseVector<TT, ALLOC>& keepVector, L&& lambda)
	{
		for(uint32 g = 0; g < keepVector.groupCount; ++g) { lambda(keepVector.groups[g]); }
	}
}
