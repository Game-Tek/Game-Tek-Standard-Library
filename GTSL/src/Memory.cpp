#include "GTSL/Memory.h"

#include <cstring>
#include <cstdlib>

using namespace GTSL;

void Allocate(const uint64 size, void** data)
{
	*data = std::malloc(size);
}

void Deallocate(const uint64 size, void* data)
{
	std::free(data);
}

void MemCopy(const uint64 size, const void* from, void* to)
{
	std::memcpy(to, from, size);
}

void SetMemory(const uint64 size, void* data, const int64 value)
{
	std::memset(data, value, size);
}

void GTSL::MemCopy(Ranger<byte> range, void* to)
{
	std::memcpy(range, to, range.Bytes());
}

void GTSL::SetMemory(Ranger<byte> range, const int64 value)
{
	std::memset(range, value, range.Bytes());
}
