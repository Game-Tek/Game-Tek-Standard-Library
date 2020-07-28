#pragma once

#include "Core.h"
#include "Assert.h"

namespace GTSL
{
	/**
	 * \brief Interface to an allocator.
	 */
	struct AllocatorReference
	{
		AllocatorReference() = default;

		AllocatorReference(const AllocatorReference& allocatorReference) = default;
		AllocatorReference(AllocatorReference&& allocatorReference) = default;

		AllocatorReference& operator=(const AllocatorReference&) = default;
		AllocatorReference& operator=(AllocatorReference&&) = default;

		/**
		 * \brief Allocates memory.
		 * \param size Number of bytes to allocate.
		 * \param alignment Alignment of the allocation.
		 * \param data Pointer to a pointer to store the allocation.
		 * \param allocatedSize Pointer to store the allocated size, this will be at least as big as size. This is done as sometimes allocators allocate more space than what it was asked to, this way the client can take advantage of this and less memory is wasted.
		 */

		/**
		 * \brief Allocates memory.
		 * \param size Number of bytes to allocate.
		 * \param alignment Alignment of the allocation.
		 * \param data Pointer to a pointer to store the allocation.
		 */

		/**
		 * \brief Deallocates allocated memory.
		 * \param size Size of the allocation being freed, can be the original size asked for, not necessarily the returned allocatedSize when allocating.
		 * \param alignment Alignment of the allocation being freed.
		 * \param data Pointer to the memory block being freed.
		 */
	};

	struct DefaultAllocatorReference
	{
		DefaultAllocatorReference() = default;

		DefaultAllocatorReference(const DefaultAllocatorReference& allocatorReference) = default;
		DefaultAllocatorReference(DefaultAllocatorReference&& allocatorReference) = default;

		DefaultAllocatorReference& operator=(const DefaultAllocatorReference&) = default;
		DefaultAllocatorReference& operator=(DefaultAllocatorReference&&) = default;

		void Allocate(uint64 size, uint64 alignment, void** data, uint64* allocated_size);
		void Deallocate(uint64 size, uint64 alignment, void* data);
	};
	
	struct Allocation
	{
		void* Data = nullptr;
	};

	template<typename T, class ALLOCATOR, typename... ARGS>
	void New(void** data, const ALLOCATOR& allocator, ARGS&&... args)
	{
		uint64 allocated_size; allocator.Allocate(sizeof(T), alignof(T), data, &allocated_size);
		::new(*data) T(GTSL::MakeForwardReference<ARGS>(args)...);
	}

	template<typename T, class ALLOCATOR>
	void Delete(void** data, const ALLOCATOR& allocator)
	{
		allocator.Deallocate(sizeof(T), alignof(T), *data);
		static_cast<T*>(*data)->~T();
		*data = nullptr;
	}
	
	template<typename T, class ALLOCATOR>
	struct SmartPointer
	{
		SmartPointer() = default;
		
		SmartPointer(const SmartPointer& other) : size(other.size), alignment(other.alignment), data(other.data)
		{
		}

		SmartPointer(SmartPointer&& other) noexcept : size(other.size), alignment(other.alignment), data(other.data)
		{
			other.data = nullptr;
		}

		void Free() { this->data->~T(); this->data = nullptr; }

		template<typename TT>
		void Free() { reinterpret_cast<TT*>(this->data)->~TT(); this->data = nullptr; }
		
		~SmartPointer() { if (this->data) { this->data->~T(); this->allocator.Deallocate(this->size, this->alignment, this->data); this->data = nullptr; } }

		SmartPointer& operator=(const SmartPointer& other)
		{
			size = other.size; alignment = other.alignment; data = other.data; return *this;
		}

		SmartPointer& operator=(SmartPointer&& other) noexcept
		{
			size = other.size; alignment = other.alignment; data = other.data;
			other.data = nullptr;
			return *this;
		}
		
		operator T*() const { return data; }
		T* operator->() const { return data; }
		T& operator*() const { return *data; }

		T* GetData() const { return data; }
		
		template<typename TT, typename... ARGS>
		static SmartPointer<T, ALLOCATOR> Create(const ALLOCATOR& allocatorReference, ARGS&&... args)
		{
			T* data = nullptr; uint64 a_s; allocatorReference.Allocate(sizeof(TT), alignof(TT), reinterpret_cast<void**>(&data), &a_s);
			::new(data) TT(GTSL::MakeForwardReference<ARGS>(args)...);
			return SmartPointer<T, ALLOCATOR>(sizeof(TT), alignof(TT), data, allocatorReference);
		}

	private:
		SmartPointer(const uint32 size, const uint32 alignment, T* object, const ALLOCATOR& allocatorReference) : size(size), alignment(alignment), data(object), allocator(allocatorReference)
		{}
		
		uint32 size{ 0 };
		uint32 alignment{ 0 };
		T* data{ nullptr };

		[[no_unique_address]] ALLOCATOR allocator;
	};
}
