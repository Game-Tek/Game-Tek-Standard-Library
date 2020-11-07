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

		T& operator[](const uint32 i) { return Elements[i]; }

		Pair<uint32, T&> At(const uint32 i) { return Pair<uint32, T&>(First + i, Elements[i]); }
		
		auto GetElements()  { return Range<T*>(ElementCount, Elements); }
		auto GetElements() const { return Range<const T*>(ElementCount, Elements); }
	};

	template<typename T>
	struct GroupIterator
	{
	public:
		GroupIterator(Group<T>* dt) : group(dt)
		{
		}

		GroupIterator(const uint32 lastElem, Group<T>* dt) : group(dt), currentElementInGroup(lastElem)
		{
		}

		bool operator!=(const GroupIterator& other) const { return currentElementInGroup != other.currentElementInGroup; }
		bool operator==(const GroupIterator& other) const { return currentElementInGroup == other.currentElementInGroup; }

		GroupIterator& operator++()
		{
			++currentElementInGroup;

			return *this;
		}

		//GVectorIterator& operator--()
		//{
		//	//while pos is not occupied
		//	do
		//	{
		//		--pos;
		//	} while (GTSL::CheckBit(pos % 32, getIndices()[pos / 32]));
		//
		//	return *this;
		//}

		Pair<uint32, T> operator*() { return Pair<uint32, T>(group->First + currentElementInGroup, group->Elements[currentElementInGroup]); }
		Pair<uint32, T> operator->() { return Pair<uint32, T>(group->First + currentElementInGroup, group->Elements + currentElementInGroup); }

		//T& operator[](const uint32 index) { return getObjects()[index]; }

		T* begin() { return group->Elements; }
		T* end() { return group->Elements + group->ElementCount; }

		Group<T>* GetGroup() { return group; }
		
	private:
		Group<T>* group;

		uint32 currentElementInGroup = 0;
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
			++currentGroup;

			return *this;
		}

		//GVectorIterator& operator--()
		//{
		//	//while pos is not occupied
		//	do
		//	{
		//		--pos;
		//	} while (GTSL::CheckBit(pos % 32, getIndices()[pos / 32]));
		//
		//	return *this;
		//}
		
		GroupIterator<T> operator*() { return GroupIterator<T>(groups + currentGroup); }
		//GroupIterator<T> operator->() { return Pair<uint32, T*>(groups[currentGroup].First + currentElementInGroup, groups[currentGroup].Elements + currentElementInGroup); }

		//T& operator[](const uint32 index) { return getObjects()[index]; }

		GroupIterator<T> begin() { return GroupIterator<T>(groups); }
		GroupIterator<T> end() { return GroupIterator<T>(groups->ElementCount, groups); }
		
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
		SparseVector() = default;

		SparseVector(const uint32 maxElements, const ALLOCATOR& allo) : allocator(allo)
		{
			uint64 allocatedSize;

			allocator.Allocate(sizeof(Group<T>) * maxElements, alignof(Group), reinterpret_cast<void**>(&groups), &allocatedSize);

			capacity = maxElements;
		}
		
		~SparseVector()
		{
			Clear();
		}
		
		void Initialize(const uint32 maxElements, const ALLOCATOR& allo)
		{
			uint64 allocatedSize;
			
			allocator = allo;
			allocator.Allocate(sizeof(Group<T>) * maxElements, alignof(Group), reinterpret_cast<void**>(&groups), &allocatedSize);

			capacity = maxElements;
		}

		template<typename... ARGS>
		void EmplaceAt(const uint32 pos, ARGS&&... args)
		{			
			for(uint32 i = 0; i < groupCount; ++i)
			{
				uint8 tag = 0;
				
				Group<T>& group = groups[i];
				
				if (pos + 1 == group.First) { tag = IS_PRE_CONTIGUOUS; }
				if (pos == group.First + group.ElementCount) { tag = IS_POST_CONTIGUOUS; }
				if (pos > group.First && pos < group.First) { tag = IS_INSIDE; }

				switch (tag)
				{
				case IS_PRE_CONTIGUOUS:
				{
					//GTSL_ASSERT(pos != group.First, "There's an element alredy using that slot")
						
					insertElement(group.GetElements(), 0);
						
					new(group.Elements) T(ForwardRef<ARGS>(args)...);

					group.First = pos;
						
					//GTSL_ASSERT(group.ElementCount != capacity, "No more space available");
						
					++group.ElementCount;
						
					return;
				}

				case IS_POST_CONTIGUOUS:
				{
					//GTSL_ASSERT(pos != group.First + group.ElementCount, "There's an element alredy using that slot")
						
					new(group.Elements + group.ElementCount) T(ForwardRef<ARGS>(args)...);
						
					//GTSL_ASSERT(group.ElementCount != capacity, "No more space available");
						
					++group.ElementCount;
					return;
				}
				case IS_INSIDE:
				{
					insertElement(group.GetElements(), group.First - pos);
					new(group.Elements + group.First - pos) T(ForwardRef<ARGS>(args)...);

					GTSL_ASSERT(group.ElementCount != capacity, "No more space available");

					++group.ElementCount;
						
					return;
				}
				default:;
				}
			}

			{
				uint64 allocatedSize;
				
				Group<T>& group = groups[groupCount];
				group.First = pos;
				group.ElementCount = 1;
				allocator.Allocate(sizeof(T) * capacity, alignof(T), reinterpret_cast<void**>(&group.Elements), &allocatedSize);

				new(group.Elements + (pos - group.First)) T(GTSL::ForwardRef<ARGS>(args)...);
				
				++groupCount;
			}
		}

		void Clear()
		{
			for(uint32 i = 0; i < groupCount; ++i)
			{
				auto& group = groups[i];
				
				for(uint32 j = 0; j < group.ElementCount; ++j)
				{
					group.Elements[j].~T();
				}

				allocator.Deallocate(sizeof(T) * capacity, alignof(T), group.Elements);
			}

			groupCount = 0;
		}

		SparseVectorIterator<T> begin() { return SparseVectorIterator<T>(groups); }
		SparseVectorIterator<T> end() { return SparseVectorIterator<T>(groupCount, groups); }

		Range<SparseVectorIterator<T>> GetRange() { return Range<SparseVectorIterator<T>>(begin(), end()); }

		[[nodiscard]] uint32 GetGroupCount() const { return groupCount; }

		auto GetGroups() const { return GTSL::Range<const Group<T>*>(groupCount, groups); }
		
		T& operator[](const uint32 element)
		{
			for(uint32 g = 0; g < groupCount; ++g)
			{
				if(groups[g].First >= element && element < groups[g].First + groups[g].ElementCount) //TODO: CAN REMOVE FIRST CONDITION IF WE CAN GUARANTEE AN ORDERED ARRAY
				{
					return groups[g].Elements[element - groups[g].First];
				}
			}
			
			GTSL_ASSERT(false, "No such element")
			return groups[0].Elements[0];
		}
		
	private:
		Group<T>* groups;

		uint32 capacity = 0;
		uint32 groupCount = 0;

		ALLOCATOR allocator;
	};
}
