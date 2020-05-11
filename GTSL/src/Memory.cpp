#include "GTSL/Memory.h"

#include <cstring>
#include <cstdlib>

using namespace GTSL;

void Memory::Allocate(const uint64 size, void** data)
{
	*data = std::malloc(size);
}

void Memory::Deallocate(const uint64 size, void* data)
{
	std::free(data);
}

void Memory::MemCopy(uint64 size, const void* from, void* to)
{
	std::memcpy(to, from, size);
}

void Memory::SetZero(uint64 size, void* data)
{
	std::memset(data, 0, size);
}
