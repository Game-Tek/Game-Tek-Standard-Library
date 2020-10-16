#pragma once
#include <GTSL/Core.h>

#include "Range.h"

namespace GTSL
{
	void ToString(int8 num,    Range<UTF8*>& buffer);
	void ToString(uint8 num,   Range<UTF8*>& buffer);
	void ToString(int16 num,   Range<UTF8*>& buffer);
	void ToString(uint16 num,  Range<UTF8*>& buffer);
	void ToString(int32 num,   Range<UTF8*>& buffer);
	void ToString(uint32 num,  Range<UTF8*>& buffer);
	void ToString(int64 num,   Range<UTF8*>& buffer);
	void ToString(uint64 num,  Range<UTF8*>& buffer);
	void ToString(float32 num, Range<UTF8*>& buffer);
	void ToString(float64 num, Range<UTF8*>& buffer);

	/**
	 * \brief Computes the length of a c string.
	 * \param text Pointer to a c string containing the string to be measured.
	 * \return The string length including the null terminator.
	 */
	constexpr uint32 StringLength(const char* text) noexcept { uint32 i{ 0 }; while (text[i] != '\0') { ++i; } return i + 1; }

	inline char ToLowerCase(char c)
	{
		if ('A' <= c && c <= 'Z') return c += ('a' - 'A');
		return c;
	}

	inline char ToUpperCase(char c)
	{
		if ('a' <= c && c <= 'z') return c += ('a' - 'A');
		return c;
	}
}
