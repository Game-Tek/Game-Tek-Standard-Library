#pragma once

#if (_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <intrin.h>
#undef BitScanForward
#endif

#include "Core.h"

namespace GTSL
{
	inline void FindFirstSetBit(uint32 number, uint8& bit) { unsigned long set_bit{ 0 }; _BitScanForward(&set_bit, number); bit = static_cast<uint8>(set_bit); }
	inline void FindFirstSetBit(uint32 number, uint8& bit, bool& anySetBit) { unsigned long set_bit{ 0 }; anySetBit = _BitScanForward(&set_bit, number); bit = static_cast<uint8>(set_bit); }
	inline void FindFirstSetBit(uint64 number, uint8& bit) { unsigned long set_bit{ 0 }; _BitScanForward64(&set_bit, number); bit = static_cast<uint8>(set_bit); }
	inline void FindFirstSetBit(uint64 number, uint8& bit, bool& anySetBit) { unsigned long set_bit{ 0 }; anySetBit = _BitScanForward64(&set_bit, number); bit = static_cast<uint8>(set_bit); }

	inline void FindLastSetBit(const uint32 number, uint8& bit, bool& anySetBit) {
		unsigned long set_bit{ 0 }; anySetBit = _BitScanReverse(&set_bit, number); bit = static_cast<uint8>(set_bit);
	}

	inline void FindLastSetBit(const uint64 number, uint8& bit, bool& anySetBit) {
		unsigned long set_bit{ 0 }; anySetBit = _BitScanReverse64(&set_bit, number); bit = static_cast<uint8>(set_bit);
	}

	inline uint8 NumberOfSetBits(const uint16 number) {
		return static_cast<uint8>(__popcnt16(number));
	}

	inline uint8 NumberOfSetBits(const uint32 number) {
		return static_cast<uint8>(__popcnt(number));
	}

	inline uint8 NumberOfSetBits(const uint64 number) {
		return static_cast<uint8>(__popcnt64(number));
	}

	template<typename T>
	consteval uint8 FindFirstSetBit(T number) {
		uint8 r = 0;
		while (number >>= 1) r++;
		return r;
	}
	
	inline void SetBit(const uint8 bitN, uint64& n) { n |= 1ULL << bitN; }
	inline void SetBit(const uint8 bitN, uint32& n) { n |= 1U << bitN; }
	inline void ClearBit(const uint8 bitN, uint64& n) { n &= ~(1ULL << bitN); }
	inline void ClearBit(const uint8 bitN, uint32& n) { n &= ~(1U << bitN); }
	inline void FlipBit(const uint8 bitN, uint64& n) { n ^= 1ULL << bitN; }
	inline void FlipBit(const uint8 bitN, uint32& n) { n ^= 1U << bitN; }
	inline bool CheckBit(const uint8 bitN, const uint64 n) { return (n >> bitN) & 1ULL; }
	inline bool CheckBit(const uint8 bitN, const uint32 n) { return (n >> bitN) & 1U; }
	inline void SetBitAs(const uint8 bitN, const bool value, uint64& n) { n = (n & ~(1ull << bitN)) | (static_cast<uint64>(value) << bitN); }
	inline void SetBitAs(const uint8 bitN, const bool value, uint32& n) { n = (n & ~(1u << bitN)) | (static_cast<uint32>(value) << bitN); }
	inline void SetBitAs(const uint8 bitN, const bool value, uint16& n) { n = (n & ~(uint16(1) << bitN)) | (static_cast<uint16>(value) << bitN); }
	inline void SetBitAs(const uint8 bitN, const bool value, uint8& n) { n = (n & ~(uint8(1) << bitN)) | (static_cast<uint8>(value) << bitN); }
	
	inline uint8 NextPowerOfTwo(uint8 number) {
		--number; number |= number >> 1; number |= number >> 2; number |= number >> 4; ++number;
		return number;
	}

	inline uint16 NextPowerOfTwo(uint16 number) {
		--number; number |= number >> 1; number |= number >> 2; number |= number >> 4; number |= number >> 8; ++number;
		return number;
	}

	inline uint32 NextPowerOfTwo(uint32 number) {
		--number; number |= number >> 1; number |= number >> 2; number |= number >> 4; number |= number >> 8; number |= number >> 16; ++number;
		return number;
	}

	inline uint64 NextPowerOfTwo(uint64 number) {
		--number; number |= number >> 1; number |= number >> 2; number |= number >> 4; number |= number >> 8; number |= number >> 16; number |= number >> 32; ++number;
		return number;
	}
}
