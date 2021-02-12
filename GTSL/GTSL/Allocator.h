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

	template<typename T, class ALLOCATOR, typename... ARGS>
	T* New(const ALLOCATOR& allocator, ARGS&&... args)
	{
		uint64 allocated_size; void* data; allocator.Allocate(sizeof(T), alignof(T), &data, &allocated_size);
		return ::new(data) T(GTSL::ForwardRef<ARGS>(args)...);
	}

	template<typename T, class ALLOCATOR>
	void Delete(T* data, const ALLOCATOR& allocator)
	{
		static_cast<T*>(data)->~T();
		allocator.Deallocate(sizeof(T), alignof(T), data);
	}
	
	template<typename T, class ALLOCATOR>
	struct SmartPointer
	{
		SmartPointer() = default;

		SmartPointer(SmartPointer&& other) noexcept : size(other.size), alignment(other.alignment), data(other.data)
		{
			other.data = nullptr;
		}

		bool TryFree() { if (data) { Free(); return true; } return false; }
		void Free() { this->data->~T(); this->data = nullptr; }
		
		~SmartPointer() { TryFree(); }

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
			::new(data) TT(GTSL::ForwardRef<ARGS>(args)...);
			return SmartPointer<T, ALLOCATOR>(sizeof(TT), alignof(TT), data, allocatorReference);
		}

	private:
		SmartPointer(const uint32 size, const uint32 alignment, T* object, const ALLOCATOR& allocatorReference) : size(size), alignment(alignment), data(object), allocator(allocatorReference)
		{}
		
		uint32 size{ 0 };
		uint32 alignment{ 0 };
		T* data{ nullptr };

#pragma warning(disable : 4648)
		[[no_unique_address]] ALLOCATOR allocator;
#pragma warning(default : 4648)
	};

	template<uint16 BYTES>
	struct StackAllocator : AllocatorReference
	{
		StackAllocator() = default;
		
		void Allocate(uint64 size, uint64 alignment, void** data, uint64* allocated_size)
		{
			GTSL_ASSERT(usedBytes <= BYTES, "Allocated more than available")
			
			*data = buffer;
			*allocated_size = BYTES;

			usedBytes += BYTES;
		}
		
		void Deallocate(const uint64 size, uint64 alignment, void* data)
		{
			GTSL_ASSERT(size <= BYTES, "Freed more bytes than could be given")
			GTSL_ASSERT(size + usedBytes <= BYTES, "Freed more bytes than could be given")
			usedBytes -= size;
		}
		
	private:
		byte buffer[BYTES];
		uint16 usedBytes = 0;
	};
}
