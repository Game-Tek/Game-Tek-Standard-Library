#include "GTSL/Id.h"

#include "GTSL/String.hpp"

using namespace GTSL;

constexpr uint32 Id32::hashString(const uint32 stringLength, const char* str) noexcept
{
	uint32 primaryHash(525410765);
	uint32 secondaryHash(0xAAAAAAAA);

	for (auto& c : Ranger(stringLength, str))
	{
		primaryHash ^= c;
		secondaryHash ^= c;
		primaryHash *= 0x5bd1e995;
		secondaryHash *= 0x80638e;
		primaryHash ^= primaryHash >> 17;
		secondaryHash ^= secondaryHash >> 29;
	}

	return ((primaryHash & 0xFFFF0000) ^ (secondaryHash & 0x0000FFFF));
}

uint16 Id16::hashString(const uint32 stringLength, const char* str)
{
	uint16 primaryHash(52541);
	uint16 secondaryHash(0xAAAA);

	for (auto& c : Ranger(stringLength, str))
	{
		primaryHash ^= c;
		secondaryHash ^= c;
		primaryHash *= 0x5bd1;
		secondaryHash *= 0x8063;
		primaryHash ^= primaryHash >> 9;
		secondaryHash ^= secondaryHash >> 11;
	}

	return ((primaryHash & 0xFF00) ^ (secondaryHash & 0x00FF));
}

constexpr Id32::Id32(const char* text) noexcept : hash(hashString(String::StringLength(text) - 1, text))
{
}

constexpr Id32::Id32(uint32 length, const char* text) noexcept : hash(hashString(length, text))
{
}

Id16::Id16(const char* text) : hash(hashString(String::StringLength(text) - 1, text))
{
}
