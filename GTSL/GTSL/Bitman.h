#pragma once
#include "Core.h"

namespace GTSL
{
	void BitScanForward(uint32 number, uint8& bit);
	void BitScanForward(uint64 number, uint8& bit);
	void BitScanForward(uint32 number, uint8& bit, bool& anySetBit);
	void BitScanForward(uint64 number, uint8& bit, bool& anySetBit);
	void BitCount(uint16 number, uint8& count);
	void BitCount(uint32 number, uint8& count);
	void BitCount(uint64 number, uint8& count);

	inline void SetBit(const uint8 bitN, uint64& n) { n |= 1ULL << bitN; }
	inline void ClearBit(const uint8 bitN, uint64& n) { n &= ~(1ULL << bitN); }
	inline void FlipBit(const uint8 bitN, uint64& n) { n ^= 1ULL << bitN; }
	inline bool CheckBit(const uint8 bitN, const uint64 n) { return (n >> bitN) & 1ULL; }
	inline void SetBitAs(const uint8 bitN, const bool value, uint64& n) { n = (n & ~(1ull << bitN)) | (static_cast<uint64>(value) << bitN); }
	
	inline void NextPowerOfTwo(uint8 number, uint8& nextPow2)
	{
		--number; number |= number >> 1; number |= number >> 2; number |= number >> 4; ++number;
		nextPow2 = number;
	}

	inline void NextPowerOfTwo(uint16 number, uint16& nextPow2)
	{
		--number; number |= number >> 1; number |= number >> 2; number |= number >> 4; number |= number >> 8; ++number;
		nextPow2 = number;
	}

	inline void NextPowerOfTwo(uint32 number, uint32& nextPow2)
	{
		--number; number |= number >> 1; number |= number >> 2; number |= number >> 4; number |= number >> 8; number |= number >> 16; ++number;
		nextPow2 = number;
	}

	inline void NextPowerOfTwo(uint64 number, uint64& nextPow2)
	{
		--number; number |= number >> 1; number |= number >> 2; number |= number >> 4; number |= number >> 8; number |= number >> 16; number |= number >> 32; ++number;
		nextPow2 = number;
	}
}
