#pragma once
#include "Core.h"

namespace GTSL
{
	void FindFirstSetBit(uint32 number, uint8& bit);
	void FindFirstSetBit(uint64 number, uint8& bit);
	void FindFirstSetBit(uint32 number, uint8& bit, bool& anySetBit);
	void FindFirstSetBit(uint64 number, uint8& bit, bool& anySetBit);
	void NumberOfSetBits(uint16 number, uint8& count);
	void NumberOfSetBits(uint32 number, uint8& count);
	void NumberOfSetBits(uint64 number, uint8& count);

	constexpr uint8 FindFirstSetBit(uint32 number)
	{
		uint8 r = 0;

		while (number >>= 1) {
			r++;
		}

		return r;
	}
	
	inline void SetBit(const uint8 bitN, uint64& n) { n |= 1ULL << bitN; }
	inline void ClearBit(const uint8 bitN, uint64& n) { n &= ~(1ULL << bitN); }
	inline void FlipBit(const uint8 bitN, uint64& n) { n ^= 1ULL << bitN; }
	inline bool CheckBit(const uint8 bitN, const uint64 n) { return (n >> bitN) & 1ULL; }
	inline void SetBitAs(const uint8 bitN, const bool value, uint64& n) { n = (n & ~(1ull << bitN)) | (static_cast<uint64>(value) << bitN); }
	inline void SetBitAs(const uint8 bitN, const bool value, uint32& n) { n = (n & ~(1u << bitN)) | (static_cast<uint32>(value) << bitN); }
	
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
