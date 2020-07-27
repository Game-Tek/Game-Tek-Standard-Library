#include "GTSL/Allocator.h"

#include "GTSL/Memory.h"

void GTSL::DefaultAllocatorReference::Allocate(const uint64 size, uint64 alignment, void** data, uint64* allocated_size)
{
	GTSL::Allocate(size, data);
	*allocated_size = size;
}

void GTSL::DefaultAllocatorReference::Deallocate(const uint64 size, uint64 alignment, void* data)
{
	GTSL::Deallocate(size, data);
}
