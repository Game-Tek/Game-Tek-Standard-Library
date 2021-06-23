#pragma once

#if (_WIN32)
#define WIN32_LEAN_AND_MEAN
#define NOCOMM
#include <Windows.h>
#include <intrin.h>
#undef BitScanForward
#endif

#include <xtr1common>

#include "Core.h"
#include "Result.h"

namespace GTSL
{
	constexpr Result<uint8> FindFirstSetBit(uint8 number) {
		if (std::is_constant_evaluated()) {
			uint8 r = 0; bool anySet = true;
			//while (number >>= 1) { r++; }
			//return Result(static_cast<uint8>(r), number != 0);

			switch (number) {
			case 0: r = 0; anySet = false; break;
			case 1 << 1: r = 1; break;
			case 1 << 2: r = 2; break;
			case 1 << 3: r = 3; break;
			case 1 << 4: r = 4; break;
			case 1 << 5: r = 5; break;
			case 1 << 6: r = 6; break;
			case 1 << 7: r = 7; break;
			}

			return Result(MoveRef(r), anySet);
		} else {
			unsigned long setBit{ 0 };
			const bool anySetBit = _BitScanForward(&setBit, number);
			return Result(static_cast<uint8>(setBit), anySetBit);
		}
	}

	inline Result<uint8> FindFirstSetBit(uint32 number) {
		unsigned long setBit{ 0 };
		const bool anySetBit = _BitScanForward(&setBit, number);
		return Result(static_cast<uint8>(setBit), anySetBit);
	}
	
	inline Result<uint8> FindFirstSetBit(uint64 number) {
		unsigned long setBit{ 0 };
		const bool anySetBit = _BitScanForward64(&setBit, number);
		return Result(static_cast<uint8>(setBit), anySetBit);
	}

	inline Result<uint8> FindFirstClearBit(uint32 number) {
		unsigned long setBit{ 0 };
		const bool anySetBit = _BitScanForward(&setBit, ~number);
		return Result(static_cast<uint8>(setBit), anySetBit);
	}
	
	inline Result<uint8> FindLastSetBit(const uint32 number) {
		unsigned long setBit{ 0 }; const bool anySetBit = _BitScanReverse(&setBit, number);
		return Result(static_cast<uint8>(setBit), anySetBit);
	}

	inline Result<uint8> FindLastSetBit(const uint64 number) {
		unsigned long setBit{ 0 }; const bool anySetBit = _BitScanReverse64(&setBit, number);
		return Result(static_cast<uint8>(setBit), anySetBit);
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

	constexpr uint8 FFSB(uint32 number) {
		uint8 r = 0;
		while (number >>= 1) r++;
		return r;
	}

	//consteval uint8 FindFirstSetBit(uint64 number) {
	//	uint8 r = 0;
	//	while (number >>= 1) r++;
	//	return r;
	//}
	
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
