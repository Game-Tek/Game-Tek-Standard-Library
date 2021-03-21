#pragma once
#include <GTSL/Core.h>

#include "Range.h"
#include "Result.h"

namespace GTSL
{
	template<uint32 L>
	class StaticString;
	
	void ToString(int8 num, Range<UTF8*>& buffer);
	void ToString(uint8 num, Range<UTF8*>& buffer);
	void ToString(int16 num, Range<UTF8*>& buffer);
	void ToString(uint16 num, Range<UTF8*>& buffer);
	void ToString(int32 num, Range<UTF8*>& buffer);
	void ToString(uint32 num, Range<UTF8*>& buffer);
	void ToString(int64 num, Range<UTF8*>& buffer);
	void ToString(uint64 num, Range<UTF8*>& buffer);
	void ToString(float32 num, Range<UTF8*>& buffer);
	void ToString(float64 num, Range<UTF8*>& buffer);

	void ToString(uint32 num, StaticString<32>& string);
	void ToString(int32 num, StaticString<32>& string);
	void ToString(float32 num, StaticString<32>& string);

	/**
	 * \brief Parses an string to return a number.
	 * \tparam T Type of the number to parse. int, uint, float
	 * \param numberString String containing JUST the number to parse. Tolerates null terminators.
	 * \return Result of the parse. Can fail.
	 */
	template<typename T>
	Result<T> ToNumber(Range<const UTF8*> numberString);

	template<>
	inline Result<uint32> ToNumber(Range<const UTF8*> numberString)
	{
		uint32 value = 0, mult = 1;

		for (uint32 j = 0, c = numberString.ElementCount() - 1; j < numberString.ElementCount(); ++j, --c)
		{
			uint8 num;

			switch (numberString[c])
			{
			case '0': num = 0; break;
			case '1': num = 1; break;
			case '2': num = 2; break;
			case '3': num = 3; break;
			case '4': num = 4; break;
			case '5': num = 5; break;
			case '6': num = 6; break;
			case '7': num = 7; break;
			case '8': num = 8; break;
			case '9': num = 9; break;
			case '\0': continue;
			default: return Result<uint32>(0, false);
			}

			value += num * mult;

			mult *= 10;
		}

		return Result<uint32>(GTSL::MoveRef(value), true);
	}

	template<>
	inline Result<int32> ToNumber(Range<const UTF8*> numberString)
	{
		int32 value = 0, mult = 1;

		for (uint32 j = 0, c = numberString.ElementCount() - 1; j < numberString.ElementCount(); ++j, --c)
		{
			uint8 num;

			switch (numberString[c])
			{
			case '0': num = 0; break;
			case '1': num = 1; break;
			case '2': num = 2; break;
			case '3': num = 3; break;
			case '4': num = 4; break;
			case '5': num = 5; break;
			case '6': num = 6; break;
			case '7': num = 7; break;
			case '8': num = 8; break;
			case '9': num = 9; break;
			case '-': num = 0; value = -value; break;
			case '\0': continue;
			default: return Result<int32>(0, false);
			}

			value += num * mult;

			mult *= 10;
		}

		return Result<int32>(GTSL::MoveRef(value), true);
	}

	template<>
	inline Result<float32> ToNumber(Range<const UTF8*> numberString)
	{
		float32 value = 0; uint32 c = numberString.ElementCount() - 1/*because of inverse parse*/; uint32 dot = 0;

		{
			float32 div = 1.0f;

			while (numberString[c--] != '.') {
				uint8 num;

				switch (numberString[c + 1])
				{
				case '0': num = 0; break;
				case '1': num = 1; break;
				case '2': num = 2; break;
				case '3': num = 3; break;
				case '4': num = 4; break;
				case '5': num = 5; break;
				case '6': num = 6; break;
				case '7': num = 7; break;
				case '8': num = 8; break;
				case '9': num = 9; break;
				case '\0': continue;
				default: return Result<float32>(0, false);
				}

				value += num * div;

				div *= 10;
			}

			value /= div;
			dot = c + 1;
		}

		float32 mult = 1;

		for (uint32 i = 0; i < dot; ++i, --c)
		{
			uint8 num;

			switch (numberString[c])
			{
			case '0': num = 0; break;
			case '1': num = 1; break;
			case '2': num = 2; break;
			case '3': num = 3; break;
			case '4': num = 4; break;
			case '5': num = 5; break;
			case '6': num = 6; break;
			case '7': num = 7; break;
			case '8': num = 8; break;
			case '9': num = 9; break;
			case '-': num = 0; value = -value; return Result<float32>(GTSL::MoveRef(value), true);
			case '\0': continue;
			default: return Result<float32>(0, false);
			}

			value += num * mult;

			mult *= 10;
		}

		return Result<float32>(GTSL::MoveRef(value), true);
	}

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

	inline bool CompareCaseInsensitive(const UTF8 a, const UTF8 b) {
		return a == ToLowerCase(b) || a == ToUpperCase(b);
	}
	
	//inline bool IsLetter(const UTF8 character)
	//{
	//	switch (character)
	//	{
	//	case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
	//	case '{': case '}': case '!': case '?': case '#': case '[': case ']':
	//	case '+': case '-': case '*': case '/': case '%':
	//	case '.': case ',': case ';':
	//	case '_':
	//	case '\"': case '\'':
	//	case '|': case '&': case '=': case '^': case '~': case '<': case '>':
	//	case ' ': case '	':
	//	case '\n': case '\0': case '\r': case '\\': case '\f': case '\b':
	//		return false;
	//	}
	//
	//	return true;
	//}

	inline bool IsLetter(const UTF8 character)
	{
		switch (character)
		{
		case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J':
		case 'K': case 'L': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
		case 'V': case 'W': case 'X': case 'Y': case 'Z':

		case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j':
		case 'k': case 'l': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
		case 'v': case 'w': case 'x': case 'y': case 'z':
			return true;
		}

		return false;
	}

	inline bool IsWhitespace(const UTF8 character) { return character == ' ' || character == '	'; }

	inline bool IsSpecialCharacter(const UTF8 character)
	{
		return character == '\n' || character == '\0' || character == '\r' || character == '\f' || character == '\b';
	}
	
	inline bool IsNumber(const UTF8 character)
	{
		switch (character)
		{
		case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
			return true;
		}

		return false;
	}

	inline bool IsSymbol(const UTF8 character)
	{
		switch (character)
		{
		case '!': case '\"': case '#': case '|': case '\'': case '$': case '%': case '&': case '/': case '(': case ')':
		case '=': case '?': case '[': case ']': case '^': case '*': case '{': case '}': case ',': case '.': case ';':
		case '<': case '>': case '_': case '~': case '-': case '+':
			return true;
		}

		return false;
	}
}
