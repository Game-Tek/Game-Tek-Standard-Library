#include "Memory.h"

#include <cstring>
#include <cstdlib>

void GTSL::Memory::Allocate(const uint64 size, void** data)
{
	*data = std::malloc(size);
}

void GTSL::Memory::Deallocate(const uint64 size, void* data)
{
	std::free(data);
}

void GTSL::Memory::CopyMemory(uint64 size, const void* from, void* to)
{
	std::memcpy(to, from, size);
}

void GTSL::Memory::SetZero(uint64 size, void* data)
{
	std::memset(data, 0, size);
}
