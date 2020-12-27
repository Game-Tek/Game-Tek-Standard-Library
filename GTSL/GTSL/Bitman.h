#pragma once
#include "Core.h"

namespace GTSL
{
	void FindFirstSetBit(uint32 number, uint8& bit);
	void FindFirstSetBit(uint64 number, uint8& bit);
	void FindFirstSetBit(uint32 number, uint8& bit, bool& anySetBit);
	void FindFirstSetBit(uint64 number, uint8& bit, bool& anySetBit);
	void FindLastSetBit(uint32 number, uint8& bit, bool& anySetBit);
	void FindLastSetBit(uint64 number, uint8& bit, bool& anySetBit);
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
	inline void SetBit(const uint8 bitN, uint32& n) { n |= 1U << bitN; }
	inline void ClearBit(const uint8 bitN, uint64& n) { n &= ~(1ULL << bitN); }
	inline void ClearBit(const uint8 bitN, uint32& n) { n &= ~(1U << bitN); }
	inline void FlipBit(const uint8 bitN, uint64& n) { n ^= 1ULL << bitN; }
	inline void FlipBit(const uint8 bitN, uint32& n) { n ^= 1U << bitN; }
	inline bool CheckBit(const uint8 bitN, const uint64 n) { return (n >> bitN) & 1ULL; }
	inline bool CheckBit(const uint8 bitN, const uint32 n) { return (n >> bitN) & 1U; }
	inline void SetBitAs(const uint8 bitN, const bool value, uint64& n) { n = (n & ~(1ull << bitN)) | (static_cast<uint64>(value) << bitN); }
	inline void SetBitAs(const uint8 bitN, const bool value, uint32& n) { n = (n & ~(1u << bitN)) | (static_cast<uint32>(value) << bitN); }
	
	inline uint8 NextPowerOfTwo(uint8 number)
	{
		--number; number |= number >> 1; number |= number >> 2; number |= number >> 4; ++number;
		return number;
	}

	inline uint16 NextPowerOfTwo(uint16 number)
	{
		--number; number |= number >> 1; number |= number >> 2; number |= number >> 4; number |= number >> 8; ++number;
		return number;
	}

	inline uint32 NextPowerOfTwo(uint32 number)
	{
		--number; number |= number >> 1; number |= number >> 2; number |= number >> 4; number |= number >> 8; number |= number >> 16; ++number;
		return number;
	}

	inline uint64 NextPowerOfTwo(uint64 number)
	{
		--number; number |= number >> 1; number |= number >> 2; number |= number >> 4; number |= number >> 8; number |= number >> 16; number |= number >> 32; ++number;
		return number;
	}
}
