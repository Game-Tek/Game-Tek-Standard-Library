#pragma once

#include "Core.h"

#include "Assert.h"

namespace GTSL
{
	template<typename T, class ALLOCATOR>
	class PagedVector
	{
	public:
		PagedVector() = default;

		void Initialize(const uint32 minPages, const uint32 minPageLength, const ALLOCATOR& allocator)
		{
			GTSL_ASSERT(pages != nullptr, "")
			dataAllocator = allocator;

			uint64 allocatedSize = 0;
			pageLength = 0xFFFFFFFF;
			
			allocator.Allocate(sizeof(T*) * minPages, alignof(T*), static_cast<void**>(&pages), &allocatedSize);
			pageCount = allocatedSize / sizeof(T*);

			for (uint32 p = 0; p < pageCount; ++p)
			{
				allocator.Allocate(sizeof(T) * minPageLength, alignof(T), static_cast<void**>(&pages[p]), &allocatedSize);
				pageLength = (allocatedSize / sizeof(T)) < pageLength ? (allocatedSize / sizeof(T)) : pageLength;
			}
		}

		template<typename... ARGS>
		void EmplaceBack(ARGS&&... args)
		{
			if (length + 1 == pageLength * pageCount) { addPage(); }
			
			auto* page = pages[length / pageLength];
			new(page + length % pageLength) T(ForwardRef<ARGS>(args)...);
			
			++length;
		}

		void Free()
		{
			GTSL_ASSERT(pageCount != 0 || pageLength != 0, "Cant clear non initialized vector")
			
			//for all full pages, run destructor on every item
			for(uint32 i = 0; i < pageCount - 1; ++i)
			{
				for(uint32 j = 0; j < pageLength - 1; ++j)
				{
					pages[i][j].~T();
				}
			}

			//run destructor on every item of the potentially partially filled last page
			for(uint32 j = 0; j < (pageLength * pageCount) % length; ++j) //CHECK
			{
				pages[pageCount - 1][j].~T();
			}

			for(uint32 p = 0; p < pageCount; ++p)
			{
				dataAllocator.Deallocate(sizeof(T) * pageLength, alignof(T), pages[p]);
			}

			dataAllocator.Deallocate(sizeof(T*) * pageCount, alignof(T*), pages);

			pages = nullptr;
		}
	private:
		T** pages = nullptr;
		uint32 pageLength = 0, pageCount = 0;
		uint32 length = 0;
		
		ALLOCATOR dataAllocator;

		void addPage()
		{}
	};
}
