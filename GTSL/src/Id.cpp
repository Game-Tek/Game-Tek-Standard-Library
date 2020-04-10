#include "Id.h"

#include "String.hpp"

#include "Ranger.h"

GTSL::Id64::Id64(const String& string) : hashValue(HashString(string))
{
}


constexpr GTSL::Id64::Id64(const char* cstring) noexcept : hashValue(HashString(cstring))
{
}

constexpr GTSL::Id64::Id64(const HashType id) noexcept : hashValue(id)
{
}

constexpr GTSL::Id64::HashType GTSL::Id64::HashString(const char* text) noexcept { return hashString(String::StringLength(text) - 1, text); };

GTSL::Id64::HashType GTSL::Id64::HashString(const String& fstring) noexcept { return hashString(fstring.GetLength(), fstring.c_str()); }

constexpr GTSL::Id64::HashType GTSL::Id64::hashString(const uint32 length, const char* text) noexcept
{
	HashType primaryHash(525201411107845655ull);
	HashType secondaryHash(0xAAAAAAAAAAAAAAAAull);
	
	for (auto& c : Ranger(length, text))
	{
		primaryHash ^= c;
		secondaryHash ^= c;
		primaryHash *= 0x5bd1e9955bd1e995;
		secondaryHash *= 0x80638e;
		primaryHash ^= primaryHash >> 47;
		secondaryHash ^= secondaryHash >> 35;
	}

	//primaryHash ^= secondaryHash + 0x9e3779b9 + (primaryHash << 6) + (primaryHash >> 2);
	
	return ((primaryHash & 0xFFFFFFFF00000000ull) ^ (secondaryHash & 0x00000000FFFFFFFFull));
}

constexpr uint32 GTSL::Id32::hashString(const uint32 stringLength, const char* str) noexcept
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

uint16 GTSL::Id16::hashString(const uint32 stringLength, const char* str)
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

constexpr GTSL::Id32::Id32(const char* text) noexcept : hash(hashString(String::StringLength(text) - 1, text))
{
}

constexpr GTSL::Id32::Id32(uint32 length, const char* text) noexcept : hash(hashString(length, text))
{
}

GTSL::Id16::Id16(const char* text) : hash(hashString(String::StringLength(text) - 1, text))
{
}
