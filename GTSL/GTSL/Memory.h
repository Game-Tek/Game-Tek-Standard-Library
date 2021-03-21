#pragma once

#include "Core.h"
#include "Range.h"

namespace GTSL
{
	template<typename T>
	concept Allocator = requires (T t, uint64 size, uint64 alignment, void** data, uint64 * aS, void* d) { t.Allocate(size, alignment, data, aS); t.Deallocate(size, alignment, d); };

	void Allocate(uint64 size, void** data);
	void Deallocate(uint64 size, void* data);
	void MemCopy(uint64 size, const void* from, void* to);
	void MemCopy(Range<byte*> range, void* to);
	void SetMemory(uint64 size, void* data, int64 value = 0);
	void SetMemory(Range<byte*> range, int64 value = 0);

	void Copy_M_32(uint64 size, const void* from, void* to);
	void Copy_M_64(uint64 size, const void* from, void* to);
	void Copy_M_128(uint64 size, const void* from, void* to);

	void ExpCopy(const uint64 size, const void* from, void* to);
	
	inline void* AlignPointer(const uint64 alignment, void* data)
	{
		return reinterpret_cast<void*>(reinterpret_cast<uint64>(static_cast<byte*>(data) + (alignment - 1)) & ~(alignment - 1));
	}

	inline byte* AlignPointer(const uint64 alignment, const byte* data) { return reinterpret_cast<byte*>(reinterpret_cast<uint64>(data) + (alignment - 1) & ~(alignment - 1)); }

	void SysAlloc(uint64 size, void** data, uint64* allocatedSize);
	void SysDealloc(uint64 size, void** data);
}
