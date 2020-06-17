#include "GTSL/Bitman.h"

#if (_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <intrin.h>
#undef BitScanForward
#endif

using namespace GTSL;

void GTSL::BitScanForward(const uint32 number, uint8& bit) { unsigned long set_bit{ 0 }; _BitScanForward(&set_bit, number); bit = static_cast<uint8>(set_bit); }

void GTSL::BitScanForward(const uint32 number, uint8& bit, bool& anySetBit) { unsigned long set_bit{ 0 }; anySetBit = _BitScanForward(&set_bit, number); bit = static_cast<uint8>(set_bit); }

void GTSL::BitScanForward(const uint64 number, uint8& bit) { unsigned long set_bit{ 0 }; _BitScanForward64(&set_bit, number); bit = static_cast<uint8>(set_bit); }

void GTSL::BitScanForward(const uint64 number, uint8& bit, bool& anySetBit) { unsigned long set_bit{ 0 }; anySetBit = _BitScanForward64(&set_bit, number); bit = static_cast<uint8>(set_bit); }

void GTSL::BitCount(const uint16 number, uint8& count) { count = static_cast<uint8>(__popcnt16(number)); }

void GTSL::BitCount(const uint32 number, uint8& count) { count = static_cast<uint8>(__popcnt(number)); }

void GTSL::BitCount(const uint64 number, uint8& count) { count = static_cast<uint8>(__popcnt64(number)); }