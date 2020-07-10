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

		AllocatorReference(void(AllocatorReference::*allocateFunc)(uint64, uint64, void**, uint64*) const, void(AllocatorReference::* deallocateFunc)(uint64, uint64, void*) const) : allocate(allocateFunc), deallocate(deallocateFunc)
		{
		}

		/**
		 * \brief Allocates memory.
		 * \param size Number of bytes to allocate.
		 * \param alignment Alignment of the allocation.
		 * \param data Pointer to a pointer to store the allocation.
		 * \param allocatedSize Pointer to store the allocated size, this will be at least as big as size. This is done as sometimes allocators allocate more space than what it was asked to, this way the client can take advantage of this and less memory is wasted.
		 */
		void Allocate(const uint64 size, const uint64 alignment, void** data, uint64 * allocatedSize) const { (this->*allocate)(size, alignment, data, allocatedSize); }

		/**
		 * \brief Allocates memory.
		 * \param size Number of bytes to allocate.
		 * \param alignment Alignment of the allocation.
		 * \param data Pointer to a pointer to store the allocation.
		 */
		void Allocate(const uint64 size, const uint64 alignment, void** data) const { uint64 allocated_size{ 0 }; (this->*allocate)(size, alignment, data, &allocated_size); }

		/**
		 * \brief Deallocates allocated memory.
		 * \param size Size of the allocation being freed, can be the original size asked for, not necessarily the returned allocatedSize when allocating.
		 * \param alignment Alignment of the allocation being freed.
		 * \param data Pointer to the memory block being freed.
		 */
		void Deallocate(const uint64 size, const uint64 alignment, void* data) const { (this->*deallocate)(size, alignment, data); }
		
	protected:
		void(AllocatorReference::*allocate)(uint64, uint64, void**, uint64*)const{ nullptr };
		void(AllocatorReference::*deallocate)(uint64, uint64, void*)const{ nullptr };
	};

	template<typename T>
	struct Allocation
	{
		Allocation() = default;
		
		Allocation(const Allocation& other) : Size(other.Size), Alignment(other.Alignment), Data(other.Data)
		{
		}

		Allocation(Allocation&& other) noexcept : Size(other.Size), Alignment(other.Alignment), Data(other.Data)
		{
			other.Data = nullptr;
		}
#if (!_DEBUG)
		~Allocation() = default;
#else
		~Allocation() { GTSL_ASSERT(!this->Data, "Data was not freed!"); }
#endif

		Allocation& operator=(const Allocation& other)
		{
			Size = other.Size; Alignment = other.Alignment; Data = other.Data; return *this;
		}

		Allocation& operator=(Allocation&& other) noexcept
		{
			Size = other.Size; Alignment = other.Alignment; Data = other.Data;
			other.Data = nullptr;
			return *this;
		}
				
		uint32 Size{ 0 };
		uint32 Alignment{ 0 };
		T* Data{ nullptr };

		operator T*() const { return Data; }
		T* operator->() const { return Data; }
		T& operator*() const { return *Data; }

		template<typename TT, typename... ARGS>
		static Allocation<T> Create(const AllocatorReference& allocatorReference, ARGS&&... args)
		{
			Allocation<T> ret; ret.Size = sizeof(TT); ret.Alignment = alignof(TT);
			allocatorReference.Allocate(sizeof(TT), alignof(TT), reinterpret_cast<void**>(&ret.Data));
			::new(ret.Data) TT(MakeForwardReference<ARGS>(args)...); return ret;
		}

		template<typename TT, typename... ARGS>
		static void Create(const AllocatorReference& allocatorReference, Allocation& allocation, ARGS&&... args)
		{
			allocation.Size = sizeof(TT); allocation.Alignment = alignof(TT);
			allocatorReference.Allocate(sizeof(TT), alignof(TT), reinterpret_cast<void**>(&allocation.Data));
			::new(allocation.Data) TT(MakeForwardReference<ARGS>(args)...);
		}
	};

	template<typename T, typename ...ARGS>
	T* New(const AllocatorReference& allocatorReference, ARGS&&... args)
	{
		void* data{ nullptr }; uint64 size{ 0 };
		allocatorReference.Allocate(sizeof(T), alignof(T), &data, &size);
		::new(data) T(MakeForwardReference<ARGS>(args)...);
		return static_cast<T*>(data);
	}
	
	template<typename T>
	void Delete(Allocation<T>& allocation, const AllocatorReference& allocatorReference)
	{
		allocation->~T();
		allocatorReference.Deallocate(allocation.Size, allocation.Alignment, static_cast<void*>(allocation.Data));
#if (_DEBUG)
		allocation.Data = nullptr;
#endif
	}

	template<typename T>
	void Delete(T* ptr, const AllocatorReference& allocatorReference)
	{
		ptr->~T();
		allocatorReference.Deallocate(sizeof(T), alignof(T), ptr);
	}
}
