#pragma once
#include "Core.h"

namespace GTSL
{
	void BitScanForward(uint32 number, uint8& bit);
	void BitScanForward(uint64 number, uint8& bit);
	void BitScanForward(uint32 number, uint8& bit, bool& anySetBit);
	void BitScanForward(uint64 number, uint8& bit, bool& anySetBit);
	void NextPowerOfTwo(uint32 number, uint32& nextPow2);
	void NextPowerOfTwo(uint64 number, uint64& nextPow2);
}
