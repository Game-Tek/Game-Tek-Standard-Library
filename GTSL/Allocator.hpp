#pragma once

#include "Core.hpp"
#include "Memory.hpp"
#include "Algorithm.hpp"
#include <type_traits>

namespace GTSL
{
	struct AllocatorReference
	{
		AllocatorReference() = default;
		AllocatorReference(const AllocatorReference& all) = default;
		AllocatorReference(AllocatorReference&& all) = default;

		AllocatorReference& operator=(const AllocatorReference&) = default;
		AllocatorReference& operator=(AllocatorReference&) = default;
	};

	struct DefaultAllocatorReference
	{
		DefaultAllocatorReference() = default;

		DefaultAllocatorReference(const DefaultAllocatorReference& allocatorReference) = default;
		DefaultAllocatorReference(DefaultAllocatorReference&& allocatorReference) = default;

		DefaultAllocatorReference& operator=(const DefaultAllocatorReference&) = default;
		DefaultAllocatorReference& operator=(DefaultAllocatorReference&&) = default;

		void Allocate(uint64 size, uint64, void** data, uint64* allocated_size)
		{
			GTSL::Allocate(size, data);
			*allocated_size = size;
		}

		void Deallocate(uint64 size, uint64, void* data)
		{
			GTSL::Deallocate(size, data);
		}
	};

	template<typename T, class ALLOCATOR, typename... ARGS>
	T* New(const ALLOCATOR& allocator, ARGS&&... args)
	{
		uint64 allocated_size; void* data; allocator.Allocate(sizeof(T), alignof(T), &data, &allocated_size);
		return ::new(data) T(GTSL::ForwardRef<ARGS>(args)...);
	}

	template<typename T, class ALLOCATOR>
	void Delete(T** data, const ALLOCATOR& allocator)
	{
		GTSL::Destroy(**data);
		allocator.Deallocate(sizeof(T), alignof(T), *data);
		*data = nullptr;
	}

	template<uint16 BYTES>
	struct StaticAllocator : AllocatorReference
	{
		StaticAllocator() = default;

		void Allocate(uint64 size, uint64, void** data, uint64* allocated_size)
		{
			GTSL_ASSERT(size <= BYTES, "Static space is less than being requested!");
			*data = buffer;
			*allocated_size = BYTES;
		}

		void Deallocate(const uint64, uint64, void*) {}

		StaticAllocator(const StaticAllocator& other) {}

		StaticAllocator& operator=(const StaticAllocator& other) { return *this; }

		StaticAllocator(StaticAllocator&&) = delete;
		StaticAllocator& operator=(StaticAllocator&&) = delete;

		byte* GetPointer() const { return buffer; }

	private:
		byte buffer[BYTES];
	};

	template<uint32 BYTES, class A>
	class DoubleAllocator {
	public:
		DoubleAllocator(const A& alloc = A()) : a(alloc) {}

		DoubleAllocator(const DoubleAllocator& other) : allocator(other.allocator), a(other.a) {}

		void Allocate(uint64 size, uint64 alignment, void** data, uint64* allocated_size)
		{
			if (size <= BYTES) 
			{
				*data = buffer;
				*allocated_size = BYTES;
				allocator = true;
			}
			else 
			{
				allocator = false;
				a.Allocate(size, alignment, data, allocated_size);
			}
		}

		void Deallocate(const uint64 size, uint64 alignment, void* data)
		{
			if (!allocator) 
			{
				a.Deallocate(size, alignment, data);
			}
		}

		const A& GetAllocator() const
		{
			return a;
		}

	private:
		bool allocator = true;
		byte buffer[BYTES]; A a;
	};

	template<typename T>
	void Allocate(auto& allocator, uint64 n, T** data)
	{
		uint64 allocatedBytes;
		allocator.Allocate(sizeof(T) * n, alignof(T), reinterpret_cast<void**>(data), &allocatedBytes);
	}

	template<typename T, std::unsigned_integral N>
	void Allocate(auto& allocator, uint64 n, T** data, N* allocatedElements, uint64 alignment = alignof(T))
	{
		uint64 allocatedBytes;
		allocator.Allocate(sizeof(T) * n, alignment, reinterpret_cast<void**>(data), &allocatedBytes);
		*allocatedElements = static_cast<N>(allocatedBytes / sizeof(T));
	}

	template<typename T>
	void Deallocate(auto& allocator, uint64 n, T* data, uint64 alignment = alignof(T))
	{
		allocator.Deallocate(sizeof(T) * n, alignment, reinterpret_cast<void*>(data));
	}

	template<typename T>
	concept Lambda = requires(T && t) { t(); };

	template<typename T, std::unsigned_integral C>
	inline void ResizeCustom(auto& allocator, T** data, C* capacity, uint64 newCapacity, auto&& f, uint64 alignment = alignof(T))
	{
		T* newAlloc; uint64 allocatedElements;
		Allocate(allocator, newCapacity, &newAlloc, &allocatedElements, alignment);

		f(*capacity, *data, allocatedElements, newAlloc);

		Deallocate(allocator, *capacity, *data);

		*data = newAlloc; *capacity = static_cast<C>(allocatedElements);
	}

	template<typename T, std::unsigned_integral C>
	inline void Resize(auto& allocator, T** data, C* capacity, uint64 newCapacity, uint64 length, uint64 alignment = alignof(T))
	{
		T* newAlloc; uint64 allocatedElements;
		Allocate(allocator, newCapacity, &newAlloc, &allocatedElements, alignment);

		Copy(length, *data, newAlloc);

		Deallocate(allocator, *capacity, *data, alignment);

		*data = newAlloc; *capacity = static_cast<C>(allocatedElements);
	}

	template<typename T, std::unsigned_integral C>
	void Resize(auto& allocator, T** data, C* capacity, uint64 newCapacity)
	{
		T* newAlloc; uint64 allocatedElements;
		Allocate(allocator, newCapacity, &newAlloc, &allocatedElements);

		Copy(*capacity, *data, newAlloc);

		Deallocate(allocator, *capacity, *data);

		*data = newAlloc;
		*capacity = allocatedElements / sizeof(T);
	}

	template<typename T, std::unsigned_integral C>
	void Resize(auto& allocator, T** data, C capacity, uint64 newCapacity)
	{
		T* newAlloc; uint64 allocatedElements;
		Allocate(allocator, newCapacity, &newAlloc, &allocatedElements);

		Copy(capacity, *data, newAlloc);

		Deallocate(allocator, capacity, *data);

		*data = newAlloc;
	}

	template<typename T, std::unsigned_integral C>
	void AllocateOrResize(auto& allocator, T** data, C* capacity, uint64 newCapacity, uint64 length, uint64 alignment = alignof(T))
	{
		T* newAlloc; uint64 allocatedElements;
		Allocate(allocator, newCapacity, &newAlloc, &allocatedElements, alignment);

		if (*data) 
		{
			Copy(length, *data, newAlloc);
			Deallocate(allocator, *capacity, *data, alignment);
		}

		*data = newAlloc; *capacity = static_cast<C>(allocatedElements);
	}
}