#pragma once

#if(_WIN64)
#include <immintrin.h>
#endif

#include "Core.h"

#include "Assert.h"
#include "Memory.h"

namespace GTSL
{
	template<typename T>
	class PagedVectorReference
	{
	public:
		PagedVectorReference(T** pages, const uint32 pageLength, const uint32 allocatedPages, const uint32 length) : pages(pages), PAGE_LENGTH(pageLength), allocatedPages(allocatedPages),
		length(length)
		{}

		[[nodiscard]] uint32 GetPageCount() const
		{
			if(length != 0)
			{
				return (length / PAGE_LENGTH) + 1;
			}

			return 0;
		}
		
		[[nodiscard]] uint32 GetLength() const
		{
			return length;
		}

		Range<T*> GetPage(const uint32 page) const
		{
			return GTSL::Range<T*>(length - (PAGE_LENGTH * page), pages[page]);
		}
		
		T& operator[](const uint32 i) const
		{
			GTSL_ASSERT(i < length, "Non valid index")
			return *iterator(i);
		}
		
	private:
		T** pages = nullptr;
		uint32 PAGE_LENGTH = 0, allocatedPages = 0;
		uint32 length = 0;

		T* iterator(const uint32 pos) const
		{
			if constexpr (_WIN64)
			{
				uint32 remainder;
				uint32 quotient = _udiv64(length, PAGE_LENGTH, &remainder);
				return pages[quotient] + remainder;
			}
			else
			{
				return pages[length / PAGE_LENGTH] + length % PAGE_LENGTH;
			}
		}
	};
	
	template<typename T, class ALLOCATOR>
	class PagedVector
	{
	public:
		PagedVector() = default;

		void Initialize(const uint32 minPages, const uint32 minPageLength, const ALLOCATOR& allocator)
		{
			GTSL_ASSERT(pages == nullptr, "")
			dataAllocator = allocator;

			uint64 allocatedSize = 0;
			PAGE_LENGTH = 0xFFFFFFFF;
			
			dataAllocator.Allocate(sizeof(T*) * minPages, alignof(T*), reinterpret_cast<void**>(&pages), &allocatedSize);
			allocatedPages = allocatedSize / sizeof(T*);

			for (uint32 p = 0; p < allocatedPages; ++p)
			{
				dataAllocator.Allocate(sizeof(T) * minPageLength, alignof(T), reinterpret_cast<void**>(&pages[p]), &allocatedSize);
				PAGE_LENGTH = (allocatedSize / sizeof(T)) < PAGE_LENGTH ? (allocatedSize / sizeof(T)) : PAGE_LENGTH;
			}
		}

		void Clear()
		{
			destroyAllChildren();
			length = 0;
		}
		
		template<typename... ARGS>
		void EmplaceBack(ARGS&&... args)
		{
			T* where;
			
			if (length == PAGE_LENGTH * allocatedPages)
			{
				addPages();
				where = pages[allocatedPages - 1];
			}
			else
			{
				where = iterator(length);
			}

			new(where) T(ForwardRef<ARGS>(args)...);
			
			++length;
		}

		void Free()
		{
			GTSL_ASSERT(allocatedPages != 0 || PAGE_LENGTH != 0, "Cant clear non initialized vector")

			destroyAllChildren();
			
			for(uint32 p = 0; p < allocatedPages; ++p)
			{
				dataAllocator.Deallocate(sizeof(T) * PAGE_LENGTH, alignof(T), pages[p]);
			}

			dataAllocator.Deallocate(sizeof(T*) * allocatedPages, alignof(T*), pages);

			pages = nullptr;
		}

		~PagedVector()
		{
			if(pages) { Free(); }
		}
		
		T& operator[](const uint32 i)
		{
			GTSL_ASSERT(i < length, "Non valid index")
			return *iterator(i);
		}

		const T& operator[](const uint32 i) const
		{
			GTSL_ASSERT(i < length, "Non valid index")
			return *iterator(i);
		}

		PagedVectorReference<T> GetReference()
		{
			return PagedVectorReference<T>(pages, PAGE_LENGTH, allocatedPages, length);
		}

		PagedVectorReference<const T> GetReference() const
		{
			return PagedVectorReference<const T>(pages, PAGE_LENGTH, allocatedPages, length);
		}
	private:
		T** pages = nullptr;
		uint32 PAGE_LENGTH = 0, allocatedPages = 0;
		uint32 length = 0;
		
		ALLOCATOR dataAllocator;

		void addPages(const uint32 count = 1)
		{
			uint64 allocatedSize;
			T** newPages;
			dataAllocator.Allocate(sizeof(T*) * (allocatedPages + count), alignof(T*), reinterpret_cast<void**>(&newPages), &allocatedSize);
			MemCopy(sizeof(T*) * allocatedPages, pages, newPages);
			dataAllocator.Deallocate(sizeof(T*) * allocatedPages, alignof(T*), pages);
			pages = newPages;

			for(uint32 i = 0, p = allocatedPages; i < count; ++i, ++p)
			{
				dataAllocator.Allocate(sizeof(T) * PAGE_LENGTH, alignof(T), reinterpret_cast<void**>(&pages[p]), &allocatedSize);
			}

			allocatedPages += count;
		}

		T* iterator(const uint32 pos) const
		{
			if constexpr (_WIN64)
			{
				uint32 remainder;
				uint32 quotient = _udiv64(length, PAGE_LENGTH, &remainder);
				return pages[quotient] + remainder;
			}
			else
			{
				return pages[length / PAGE_LENGTH] + length % PAGE_LENGTH;
			}
		}

		void destroyAllChildren()
		{
			const uint32 fullyUsedPagesCount = length / PAGE_LENGTH;

			//for all full pages, run destructor on every item
			for (uint32 i = 0; i < fullyUsedPagesCount; ++i)
			{
				for (uint32 j = 0; j < PAGE_LENGTH; ++j) { pages[i][j].~T(); }
			}

			//run destructor on every item of the potentially partially filled last page
			for (uint32 j = 0; j < length - (fullyUsedPagesCount * PAGE_LENGTH); ++j) //CHECK
			{
				pages[fullyUsedPagesCount][j].~T();
			}
		}
	};
}
