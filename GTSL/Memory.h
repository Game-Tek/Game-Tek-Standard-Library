#pragma once

#include "Core.h"
#include "Range.hpp"

#include <memory>

#if (_WIN64)
#define WIN32_LEAN_AND_MEAN
#define NO_COMM
#include <Windows.h>
#endif
#include <immintrin.h>
#include <GTSL/Math/Math.hpp>

namespace GTSL
{
	template<typename T>
	concept Allocator = requires (T t, uint64 size, uint64 alignment, void** data, uint64 * aS, void* d) { t.Allocate(size, alignment, data, aS); t.Deallocate(size, alignment, d); };

	inline void Allocate(uint64 size, void** data) {
		*data = std::malloc(size);
	}

	inline void Deallocate(uint64 size, void* data) {
		std::free(data);
	}

	inline void MemCopy(uint64 size, const void* from, void* to) {
		std::memcpy(to, from, size);
	}

	inline void MemCopy(Range<byte*> range, void* to) {
		std::memcpy(to, range.begin(), range.Bytes());
	}

	inline void SetMemory(uint64 size, void* data, int32 value = 0) {
		std::memset(data, value, size);
	}

	inline void SetMemory(Range<byte*> range, int32 value = 0) {
		std::memset(range.begin(), value, range.Bytes());
	}

	inline void Copy_M_128(uint64 size, const void* from, void* to)
	{
		GTSL_ASSERT(size % 128 == 0, "Not perfect");
		__m256i vector[4];
		for (uint32 i = 0; i < size / 128; ++i) {
			vector[0] = _mm256_loadu_epi8(static_cast<const byte*>(from) + (i * 128));
			vector[1] = _mm256_loadu_epi8(static_cast<const byte*>(from) + (i * 128) + 32);
			vector[2] = _mm256_loadu_epi8(static_cast<const byte*>(from) + (i * 128) + 64);
			vector[3] = _mm256_loadu_epi8(static_cast<const byte*>(from) + (i * 128) + 128);

			_mm256_storeu_epi8(static_cast<byte*>(to) + (i * 128), vector[0]);
			_mm256_storeu_epi8(static_cast<byte*>(to) + (i * 128) + 32, vector[1]);
			_mm256_storeu_epi8(static_cast<byte*>(to) + (i * 128) + 64, vector[2]);
			_mm256_storeu_epi8(static_cast<byte*>(to) + (i * 128) + 128, vector[3]);
		}
	}

	inline void* AlignPointer(const uint64 alignment, void* data)
	{
		return reinterpret_cast<void*>(reinterpret_cast<uint64>(static_cast<byte*>(data) + (alignment - 1)) & ~(alignment - 1));
	}

	inline byte* AlignPointer(const uint64 alignment, const byte* data) { return reinterpret_cast<byte*>(reinterpret_cast<uint64>(data) + (alignment - 1) & ~(alignment - 1)); }

	inline void SysAlloc(uint64 size, void** data, uint64* allocatedSize) {
		DWORD allocationInfo = 0;

		allocationInfo |= MEM_COMMIT; allocationInfo |= MEM_RESERVE;

		auto* allocatedAddress = VirtualAlloc(nullptr, size, allocationInfo, 0);

		SYSTEM_INFO systemInfo;
		GetSystemInfo(&systemInfo);

		*allocatedSize = GTSL::Math::RoundUpByPowerOf2(size, systemInfo.dwPageSize);
		*data = allocatedAddress;
	}

	inline void SysDealloc(uint64 size, void** data) {
		VirtualFree(*data, 0, MEM_RELEASE);
	}
}
