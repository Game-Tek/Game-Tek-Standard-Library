#include "GTSL/Bitscan.h"

#include <algorithm>
#if (_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef BitScanForward
#endif

using namespace GTSL;

void GTSL::BitScanForward(const uint32 number, uint8& bit)
{
    unsigned long set_bit{ 0 };	_BitScanForward(&set_bit, number); bit = set_bit;
}

void GTSL::BitScanForward(const uint64 number, uint8& bit)
{
	unsigned long set_bit{ 0 };	_BitScanForward64(&set_bit, number); bit = set_bit;
}

void GTSL::NextPowerOfTwo(uint32 number, uint32& nextPow2)
{
    --number;
    number |= number >> 1; number |= number >> 2; number |= number >> 4; number |= number >> 8; number |= number >> 16;
    ++number;
	
    nextPow2 = number;
}

void GTSL::NextPowerOfTwo(uint64 number, uint64& nextPow2)
{
    --number;
    number |= number >> 1; number |= number >> 2; number |= number >> 4; number |= number >> 8; number |= number >> 16; number |= number >> 32;
    ++number;
	
    nextPow2 = number;
}