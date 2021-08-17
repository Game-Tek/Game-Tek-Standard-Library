#pragma once

#include "Core.h"

#include "Assert.h"
#include "Bitman.h"
#include "Memory.h"

namespace GTSL
{
	template<typename T, class ALLOCATOR>
	class PagedVector;
	
	template<typename T, class ALLOCATOR>
	class PagedVectorIterator
	{
	public:
		PagedVectorIterator(PagedVector<T, ALLOCATOR>* vector) : vector(vector)
		{}

		T& operator*() const;

		PagedVectorIterator operator++();

		bool operator!=(const bool) const;

	private:
		PagedVector<T, ALLOCATOR>* vector = nullptr;
		uint32 page = 0, element = 0, pos = 0;

		static constexpr uint32 modulo(const uint32 key, const uint32 size) { return key & (size - 1); }
	};

	template<typename T, class ALLOCATOR>
	class PagedVector
	{
	public:
		PagedVector() = default;

		//PagedVector(const ALLOCATOR& allocator) : dataAllocator(allocator)
		//{
		//}
		
		PagedVector(const uint32 minPages, const uint32 minPageLength, const ALLOCATOR& allocator) : dataAllocator(allocator), PAGE_LENGTH(NextPowerOfTwo(minPageLength))
		{
			GTSL_ASSERT(pages == nullptr, "")

			uint64 allocatedSize = 0;
			
			dataAllocator.Allocate(sizeof(T*) * minPages, alignof(T*), reinterpret_cast<void**>(&pages), &allocatedSize);
			allocatedPages = static_cast<uint32>(allocatedSize / sizeof(T*));

			for (uint32 p = 0; p < allocatedPages; ++p) {
				dataAllocator.Allocate(sizeof(T) * PAGE_LENGTH, alignof(T), reinterpret_cast<void**>(&pages[p]), &allocatedSize);
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

		PagedVectorIterator<T, ALLOCATOR> begin() {
			return PagedVectorIterator<T, ALLOCATOR>(this);
		}

		[[nodiscard]] bool end() const { return true; }
	private:
		ALLOCATOR dataAllocator;
		
		T** pages = nullptr;
		//is a power of 2
		uint32 PAGE_LENGTH = 0;
		uint32 allocatedPages = 0;
		uint32 length = 0;		

		static constexpr uint32 modulo(const uint32 key, const uint32 size) { return key & (size - 1); }
		
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

		T* iterator(const uint32 pos) const {
			return pages[pos / PAGE_LENGTH] + modulo(pos, PAGE_LENGTH);
		}

		void destroyAllChildren()
		{
			const uint32 fullyUsedPagesCount = length / PAGE_LENGTH;

			//for all full pages, run destructor on every item
			for (uint32 i = 0; i < fullyUsedPagesCount; ++i) { for (uint32 j = 0; j < PAGE_LENGTH; ++j) { pages[i][j].~T(); } }

			//run destructor on every item of the potentially partially filled last page
			for (uint32 j = 0; j < modulo(length, PAGE_LENGTH); ++j) { pages[fullyUsedPagesCount][j].~T(); }
		}

		friend PagedVectorIterator<T, ALLOCATOR>;
	};

	template <typename T, class ALLOCATOR>
	T& PagedVectorIterator<T, ALLOCATOR>::operator*() const {
		return vector->pages[page][element];
	}

	template <typename T, class ALLOCATOR>
	PagedVectorIterator<T, ALLOCATOR> PagedVectorIterator<T, ALLOCATOR>::operator++() {
		++pos;
		page = pos / vector->PAGE_LENGTH;
		element = modulo(pos, vector->PAGE_LENGTH);
		return *this;
	}

	template <typename T, class ALLOCATOR>
	bool PagedVectorIterator<T, ALLOCATOR>::operator!=(const bool) const { return pos != vector->length; }
}
