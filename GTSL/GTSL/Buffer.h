#pragma once

#include "Core.h"

#include "Allocator.h"

namespace GTSL
{
	class Buffer
	{
		byte* data{ nullptr };
		uint32 capacity{ 0 };
		
	public:
		Buffer() = default;
		~Buffer() = default;
		
		void Allocate(const uint32 bytes, const uint32 alignment, AllocatorReference* allocatorReference)
		{
			uint64 allocated_size{ 0 };
			allocatorReference->Allocate(bytes, alignment, reinterpret_cast<void**>(&data), &allocated_size);
			capacity = allocated_size;
		}

		void Free(const uint32 alignment, AllocatorReference* allocatorReference)
		{
			allocatorReference->Deallocate(capacity, alignment, data);
			capacity = 0;
		}

		[[nodiscard]] uint32 GetCapacity() const { return capacity; }
		[[nodiscard]] byte* GetData() const { return data; }
	};
}
