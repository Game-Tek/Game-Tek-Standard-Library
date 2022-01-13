#pragma once

#include "Algorithm.hpp"
#include "Core.h"
#include "Assert.h"
#include "FunctionPointer.hpp"
#include "Memory.h"

#include <type_traits>

namespace GTSL
{
	/**
	 * \brief Interface to an allocator.
	 */
	struct AllocatorReference {
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

	struct DefaultAllocatorReference {
		DefaultAllocatorReference() = default;

		DefaultAllocatorReference(const DefaultAllocatorReference& allocatorReference) = default;
		DefaultAllocatorReference(DefaultAllocatorReference&& allocatorReference) = default;

		DefaultAllocatorReference& operator=(const DefaultAllocatorReference&) = default;
		DefaultAllocatorReference& operator=(DefaultAllocatorReference&&) = default;

		void Allocate(uint64 size, uint64 alignment, void** data, uint64* allocated_size) {
			GTSL::Allocate(size, data);
			*allocated_size = size;
		}
		
		void Deallocate(uint64 size, uint64 alignment, void* data) {
			GTSL::Deallocate(size, data);
		}
	};

	template<typename T, class ALLOCATOR, typename... ARGS>
	T* New(const ALLOCATOR& allocator, ARGS&&... args) {
		uint64 allocated_size; void* data; allocator.Allocate(sizeof(T), alignof(T), &data, &allocated_size);
		return ::new(data) T(GTSL::ForwardRef<ARGS>(args)...);
	}

	template<typename T, class ALLOCATOR>
	void Delete(T** data, const ALLOCATOR& allocator) {
		GTSL::Destroy(**data);
		allocator.Deallocate(sizeof(T), alignof(T), *data);
		*data = nullptr;
	}
	
	template<typename T, class ALLOCATOR>
	struct SmartPointer {
		SmartPointer() {}

		template<typename... ARGS>
		SmartPointer(const ALLOCATOR& alloc, ARGS&&... args) : allocator(alloc) {
			uint64 allocatedSize = 0;
			allocator.Allocate(sizeof(T), alignof(T), reinterpret_cast<void**>(&data), &allocatedSize);
			size = static_cast<uint32>(allocatedSize);
			alignment = static_cast<uint32>(alignof(T));
			::new(data) T(ForwardRef<ARGS>(args)...);

			auto dest = [](void* data) {
				Destroy(*static_cast<T*>(data));
			};
			
			destructor = dest;
		}
		
		template<typename TT>
		SmartPointer(SmartPointer<TT, ALLOCATOR>&& other) noexcept : allocator(MoveRef(other.allocator)), size(other.size), alignment(other.alignment), data(reinterpret_cast<T*>(other.data)), destructor(other.destructor) {
			other.data = nullptr;
		}
		
		SmartPointer(SmartPointer&& other) noexcept : allocator(MoveRef(other.allocator)), size(other.size), alignment(other.alignment), data(other.data), destructor(other.destructor) {
			other.data = nullptr;
		}

		bool TryFree() { if (data) { Free(); return true; } return false; }
		
		void Free() {
			destructor(data);
			allocator.Deallocate(size, alignment, reinterpret_cast<void*>(data));
			data = nullptr;
		}
		
		~SmartPointer() { TryFree(); }

		template<typename TT>
		SmartPointer& operator=(SmartPointer<TT, ALLOCATOR>&& other) noexcept {
			TryFree();
			size = other.size; alignment = other.alignment; data = reinterpret_cast<T*>(other.data); destructor = other.destructor;
			other.data = nullptr;
			return *this;
		}
		
		SmartPointer& operator=(SmartPointer&& other) noexcept {
			TryFree();
			allocator = MoveRef(other.allocator); size = other.size; alignment = other.alignment; data = other.data; destructor = other.destructor;
			other.data = nullptr;
			return *this;
		}
		
		operator T*() const { return data; }
		T* operator->() const { return data; }
		T& operator*() const { return *data; }

		T* GetData() const { return data; }
	
	private:
#pragma warning(disable : 4648)
		[[no_unique_address]] ALLOCATOR allocator;
#pragma warning(default : 4648)
		
		uint32 size = 0;
		uint32 alignment = 0;
		T* data = nullptr;
		void(*destructor)(void*) = nullptr;

		friend struct SmartPointer;	
	public:
		friend SmartPointer<T, ALLOCATOR> Create(const T& obj, const ALLOCATOR& allocator = ALLOCATOR());
	};

	//template<typename T, class ALLOCATOR>
	//inline SmartPointer<T, ALLOCATOR> Create(const T& obj, const ALLOCATOR& allocator = ALLOCATOR())
	//{
	//	return SmartPointer<T, ALLOCATOR>(allocator).make<T>(obj);
	//}
	
	template<uint16 BYTES>
	struct StaticAllocator : AllocatorReference
	{
		StaticAllocator() = default;
		
		void Allocate(uint64 size, uint64 alignment, void** data, uint64* allocated_size) {
			GTSL_ASSERT(size <= BYTES, "Static space is less than being requested!");
			*data = buffer;
			*allocated_size = BYTES;
		}
		
		void Deallocate(const uint64 size, uint64 alignment, void* data) {}

		StaticAllocator(const StaticAllocator& other) {}

		StaticAllocator& operator=(const StaticAllocator& other) { return *this; }

		StaticAllocator(StaticAllocator&&) = delete;
		StaticAllocator& operator=(StaticAllocator&&) = delete;

		byte* GetPointer() const { return buffer; }

	private:
		byte buffer[BYTES];
	};

	template<uint32 BYTES, class A>
	class DoubleAllocator
	{
	public:
		DoubleAllocator(const A& alloc = A()) : a(alloc) {}

		DoubleAllocator(const DoubleAllocator& other) : allocator(other.allocator), a(other.a) {}
		
		void Allocate(uint64 size, uint64 alignment, void** data, uint64* allocated_size)
		{
			if(size <= BYTES) {
				*data = buffer;
				*allocated_size = BYTES;
				allocator = true;
			} else {
				allocator = false;
				a.Allocate(size, alignment, data, allocated_size);
			}
		}

		void Deallocate(const uint64 size, uint64 alignment, void* data)
		{
			if(!allocator) {
				a.Deallocate(size, alignment, data);
			}
		}

	private:
		bool allocator = true;
		byte buffer[BYTES]; A a;
	};

	template<typename T>
	void Allocate(auto& allocator, uint64 n, T** data) {
		uint64 allocatedBytes;
		allocator.Allocate(sizeof(T) * n, alignof(T), reinterpret_cast<void**>(data), &allocatedBytes);
	}

	template<typename T, std::unsigned_integral N>
	void Allocate(auto& allocator, uint64 n, T** data, N* allocatedElements, uint64 alignment = alignof(T)) {
		uint64 allocatedBytes;
		allocator.Allocate(sizeof(T) * n, alignment, reinterpret_cast<void**>(data), &allocatedBytes);
		*allocatedElements = static_cast<N>(allocatedBytes / sizeof(T));
	}

	template<typename T>
	void Deallocate(auto& allocator, uint64 n, T* data, uint64 alignment = alignof(T)) {
		allocator.Deallocate(sizeof(T) * n, alignment, reinterpret_cast<void*>(data));
	}

	template<typename T>
	concept Lambda = requires(T && t) { t.operator(); };

	template<typename T, std::unsigned_integral C>
	void Resize(auto& allocator, T** data, C* capacity, uint64 newCapacity, Lambda auto&& f, uint64 alignment = alignof(T)) {
		T* newAlloc; uint64 allocatedElements;
		Allocate(allocator, newCapacity, &newAlloc, &allocatedElements, alignment);

		f(*capacity, *data, allocatedElements, newAlloc);

		Deallocate(allocator, *capacity, *data);

		*data = newAlloc; *capacity = static_cast<C>(allocatedElements);
	}

	template<typename T, std::unsigned_integral C>
	void Resize(auto& allocator, T** data, C* capacity, uint64 newCapacity, uint64 length, uint64 alignment = alignof(T)) {
		T* newAlloc; uint64 allocatedElements;
		Allocate(allocator, newCapacity, &newAlloc, &allocatedElements, alignment);

		Copy(length, *data, newAlloc);

		Deallocate(allocator, *capacity, *data, alignment);

		*data = newAlloc; *capacity = static_cast<C>(allocatedElements);
	}

	template<typename T, std::unsigned_integral C>
	void Resize(auto& allocator, T** data, C* capacity, uint64 newCapacity) {
		T* newAlloc; uint64 allocatedElements;
		Allocate(allocator, newCapacity, &newAlloc, &allocatedElements);

		Copy(*capacity, *data, newAlloc);

		Deallocate(allocator, *capacity, *data);

		*data = newAlloc;
		*capacity = allocatedElements / sizeof(T);
	}

	template<typename T, std::unsigned_integral C>
	void Resize(auto& allocator, T** data, C capacity, uint64 newCapacity) {
		T* newAlloc; uint64 allocatedElements;
		Allocate(allocator, newCapacity, &newAlloc, &allocatedElements);

		Copy(capacity, *data, newAlloc);

		Deallocate(allocator, capacity, *data);

		*data = newAlloc;
	}
}
