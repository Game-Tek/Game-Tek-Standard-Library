#pragma once

#include "Core.h"
#include "Ranger.h"

namespace GTSL
{
	void Allocate(uint64 size, void** data);
	void Deallocate(uint64 size, void* data);
	void MemCopy(uint64 size, const void* from, void* to);
	void MemCopy(Ranger<byte> range, void* to);
	void SetMemory(uint64 size, void* data, int64 value = 0);
	void SetMemory(Ranger<byte> range, int64 value = 0);

	inline void* AlignPointer(const uint64 alignment, void* data)
	{
		return reinterpret_cast<void*>(reinterpret_cast<uint64>(static_cast<byte*>(data) + (alignment - 1)) & ~(alignment - 1));
	}

	inline byte* AlignPointer(const uint64 alignment, const byte* data) { return reinterpret_cast<byte*>(reinterpret_cast<uint64>(data) + (alignment - 1) & ~(alignment - 1)); }
}
