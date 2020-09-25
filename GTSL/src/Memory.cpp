#include "GTSL/Memory.h"

#include <cstring>
#include <cstdlib>

using namespace GTSL;

void GTSL::Allocate(const uint64 size, void** data)
{
	*data = std::malloc(size);
}

void GTSL::Deallocate(uint64 size, void* data)
{
	std::free(data);
}

void GTSL::MemCopy(const uint64 size, const void* from, void* to)
{
	std::memcpy(to, from, size);
}

void GTSL::MemCopy(Range<byte*> range, void* to)
{
	std::memcpy(to, range.begin(), range.Bytes());
}

void GTSL::SetMemory(const uint64 size, void* data, const int64 value)
{
	std::memset(data, value, size);
}

void GTSL::SetMemory(Range<byte*> range, const int64 value)
{
	std::memset(range.begin(), value, range.Bytes());
}
