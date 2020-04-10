#pragma once
#include "Core.h"

namespace GTSL
{
	static void BitScanForward(uint32 number, uint8& bit);
	static void BitScanForward(uint64 number, uint8& bit);
	static void NextPowerOfTwo(uint32 number, uint32& nextPow2);
	static void NextPowerOfTwo(uint64 number, uint64& nextPow2);
	//static void BitTest(uint64 var, uint8 bit, bool& result);
}
