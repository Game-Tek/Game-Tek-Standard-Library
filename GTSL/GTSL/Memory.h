#pragma once

#include "Core.h"

namespace GTSL
{
	class Memory
	{
	public:
		static void Allocate(uint64 size, void** data);
		static void Deallocate(uint64 size, void* data);
		static void MemCopy(uint64 size, const void* from, void* to);
		static void SetZero(uint64 size, void* data);

		static void* AlignedPointer(const uint64 alignment, void* data)
		{
			return reinterpret_cast<void*>(reinterpret_cast<uint64>(static_cast<byte*>(data) + (alignment - 1)) & ~(alignment - 1));
		}
	};
}