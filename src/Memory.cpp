#include "GTSL/Memory.h"

#include <cstring>
#include <cstdlib>
#include <immintrin.h>

#include <Windows.h>

#include "GTSL/Bitman.h"
#include "GTSL/Math/Math.hpp"

using namespace GTSL;

void GTSL::MemCopy(Range<byte*> range, void* to)
{
	std::memcpy(to, range.begin(), range.Bytes());
}

void GTSL::SetMemory(const uint64 size, void* data, const int32 value)
{
	std::memset(data, value, size);
}

void GTSL::SetMemory(Range<byte*> range, const int32 value)
{
	std::memset(range.begin(), value, range.Bytes());
}

void GTSL::ExpCopy(const uint64 size, const void* from, void* to)
{
	auto perByte = size;
	
	for (uint32 i = 0; i < size / 128; ++i)
	{
		const auto modSize = (size % 128);
		Copy_M_128(size - modSize, from, to);

		perByte -= 128;

		from = static_cast<const byte*>(from) + modSize;
		to = static_cast<byte*>(to) + modSize;
	}

	for (uint32 i = 0; i < perByte / 64; ++i)
	{
		const auto modSize = (size % 64);
		Copy_M_64(size - modSize, from, to);

		perByte -= 64;

		from = static_cast<const byte*>(from) + modSize;
		to = static_cast<byte*>(to) + modSize;
	}
	
	for(uint32 i = 0; i < perByte / 32; ++i)
	{
		const auto modSize = (size % 32);
		Copy_M_32(size - modSize, from, to);

		perByte -= 32;

		from = static_cast<const byte*>(from) + modSize;
		to = static_cast<byte*>(to) + modSize;
	}

	for(uint32 i = 0; i < perByte; ++i)
	{
		const byte b = *static_cast<const byte*>(from);
		*static_cast<byte*>(to) = b;
	}
}

void GTSL::SysAlloc(uint64 size, void** data, uint64* allocatedSize)
{
	DWORD allocationInfo = 0;

	allocationInfo |= MEM_COMMIT; allocationInfo |= MEM_RESERVE;
	
	auto* allocatedAddress = VirtualAlloc(nullptr, size, allocationInfo, 0);

	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);

	*allocatedSize = GTSL::Math::RoundUpByPowerOf2(size, systemInfo.dwPageSize);
	*data = allocatedAddress;
}

void GTSL::SysDealloc(uint64 size, void** data)
{
	VirtualFree(*data, 0, MEM_RELEASE);
}

void GTSL::Copy_M_32(uint64 size, const void* from, void* to)
{
	GTSL_ASSERT(size % 32 == 0, "Not perfect")
	
	__m256i vector;
	
	for(uint32 i = 0; i < size / 32; ++i)
	{
		vector = _mm256_loadu_epi8(static_cast<const byte*>(from) + (i * 32u));
		_mm256_storeu_epi8(static_cast<byte*>(to) + (i * 32u), vector);
	}
	
}

void GTSL::Copy_M_64(uint64 size, const void* from, void* to)
{
	GTSL_ASSERT(size % 64 == 0, "Not perfect")
	
	__m256i vector[2];

	for (uint32 i = 0; i < size / 64; ++i)
	{
		vector[0] = _mm256_loadu_epi8(static_cast<const byte*>(from) + (i * 64));
		vector[1] = _mm256_loadu_epi8(static_cast<const byte*>(from) + (i * 64) + 32);

		_mm256_storeu_epi8(static_cast<byte*>(to) + (i * 64), vector[0]);
		_mm256_storeu_epi8(static_cast<byte*>(to) + (i * 64) + 32, vector[1]);
	}
}

void GTSL::Copy_M_128(uint64 size, const void* from, void* to)
{
	GTSL_ASSERT(size % 128 == 0, "Not perfect")
	
	__m256i vector[4];

	for (uint32 i = 0; i < size / 128; ++i)
	{
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
