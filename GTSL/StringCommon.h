#pragma once
#include <GTSL/Core.h>

#include "Range.h"
#include "Result.h"
#include "Tuple.h"
#include "Math/Math.hpp"
#include "Unicode.hpp"

namespace GTSL
{
	/**
	* \brief Computes the length of a c string.
	* \param text Pointer to a c string containing the string to be measured.
	* \return The string length including the null terminator.
	*/
	constexpr uint32 StringByteLength(const char8_t* text) noexcept { uint32 i{ 0 }; while (text[i] != '\0') { ++i; } return i + 1; }

	constexpr Pair<uint32, uint32> StringLengths(const char8_t* text) noexcept { 
		uint32 bytes = 0, cp = 0;
		while (text[bytes] != '\0') { bytes += UTF8CodePointLength(text[bytes]); ++cp; }
		return { bytes + 1, cp + 1 };
	}

	constexpr Pair<uint32, uint32> StringLengths(Range<const char8_t*> text) noexcept {
		uint32 bytes = 0, cp = 0;
		for (; bytes < text.ElementCount();) { bytes += UTF8CodePointLength(text[bytes]); ++cp; }
		return { bytes, cp };
	}

	inline Tuple<uint32, uint32, uint8> StringLengths2(const char8_t* text) noexcept {
		uint32 codePoint = 0; uint32 byt = 0; uint8 len = 1;

		while (text[byt] != u8'\0') {
			++codePoint;
			len = UTF8CodePointLength(text[byt]);
			byt += len;
		}

		return Tuple(MoveRef(byt) + 1, MoveRef(codePoint) + 1, MoveRef(len));
	}

	inline Tuple<uint32, uint32, uint8> StringLengths2(Range<const char8_t*> text) noexcept {
		return StringLengths2(text.begin());
	}
	
	template<class S>
	void ToString(const char* str, S& string) {
		string += Range<const char8_t*>(StringByteLength(reinterpret_cast<const char8_t*>(str)), reinterpret_cast<const char8_t*>(str));
	}

	template<class S>
	void ToString(const char8_t* str, S& string) {
		string += Range<const char8_t*>(StringByteLength(str), str);
	}

	template<class S>
	void ToString(const Range<const char8_t*> str, S& string) {
		string += str;
	}

	template<class S>
	void ToString(uint32 num, S& string)
	{
		uint8 i = 30, len = 0;

		char8_t str[32]; str[31] = '\0';

		do {
			// Process individual digits 
			str[i--] = (num % 10) + '0';
			num /= 10;
			++len;
		} while (num);

		string += GTSL::Range<const char8_t*>(len + 1, str + i + 1);
	}

	template<class S>
	void ToString(uint64 num, S& string) {
		uint8 i = 30, len = 0;

		char8_t str[32]; str[31] = '\0';

		do {
			// Process individual digits 
			str[i--] = (num % 10) + '0';
			num /= 10;
			++len;
		} while (num);

		string += GTSL::Range<const char8_t*>(len + 1, str + i + 1);
	}

	template<class S>
	void ToString(int32 num, S& string)
	{
		uint8 i = 30, len = 0;

		char8_t str[32]; str[31] = '\0';

		do {
			// Process individual digits 
			str[i--] = (num % 10) + '0';
			num /= 10;
			++len;
		} while (num);

		if (num < 0) { str[i--] = '-'; ++len; }

		string += GTSL::Range<const char8_t*>(len + 1, str + i + 1);
	}

	template<class S>
	void ToString(float32 num, S& string)
	{
		uint8 i = 30, len = 0;
		char8_t str[32]; str[31] = '\0'; bool isNegative = false;

		if (num < 0.0f) { num = -num; isNegative = true; }

		// Extract integer part 
		int32 ipart = static_cast<int32>(num);

		// Extract floating part 
		float32 fpart = num - static_cast<float32>(ipart);

		auto intToStr = [&](int32 integer)
		{
			do {
				// Process individual digits 
				str[i--] = (integer % 10) + '0';
				num /= 10;
				++len;
			} while (num);
		};

		// check for display option after point 
		if (6 != 0) {
			// Get the value of fraction part upto given no. 
			// of points after dot. The third parameter  
			// is needed to handle cases like 233.007 
			fpart = fpart * Math::Power(10.0f, 6.0f);

			intToStr(static_cast<int32>(fpart));

			str[i--] = '.'; // add dot
			++len;
		}

		// convert integer part to string 
		intToStr(ipart);

		if (isNegative) { str[i--] = '-'; ++len; }

		string += GTSL::Range<const char8_t*>(len + 1, str + i + 1);
	}

	
	/**
	 * \brief Parses an string to return a number.
	 * \tparam T Type of the number to parse. int, uint, float
	 * \param numberString String containing JUST the number to parse. Tolerates null terminators.
	 * \return Result of the parse. Can fail.
	 */
	template<typename T>
	Result<T> ToNumber(Range<const char8_t*> numberString);

	template<>
	inline Result<uint32> ToNumber(Range<const char8_t*> numberString) {
		uint32 value = 0, mult = 1;

		for (uint64 j = 0, c = numberString.ElementCount() - 1; j < numberString.ElementCount(); ++j, --c) {
			uint8 num;

			switch (numberString[c]) {
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
			default: return Result(0u, false);
			}

			value += num * mult;

			mult *= 10;
		}

		return Result(MoveRef(value), true);
	}

	template<>
	inline Result<int32> ToNumber(Range<const char8_t*> numberString) {
		int32 value = 0, mult = 1;

		for (uint64 j = 0, c = numberString.ElementCount() - 1; j < numberString.ElementCount(); ++j, --c) {
			uint8 num;

			switch (numberString[c]) {
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
			default: return Result(0, false);
			}

			value += num * mult;

			mult *= 10;
		}

		return Result(MoveRef(value), true);
	}

	template<>
	inline Result<float32> ToNumber(Range<const char8_t*> numberString) {
		float32 value = 0; uint64 c = numberString.ElementCount() - 1/*because of inverse parse*/; uint64 dot;

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

		for (uint64 i = 0; i < dot; ++i, --c) {
			uint8 num;

			switch (numberString[c]) {
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
			case '-': num = 0; value = -value; return Result(MoveRef(value), true);
			case '\0': continue;
			default: return Result(0.0f, false);
			}

			value += num * mult;

			mult *= 10;
		}

		return Result(MoveRef(value), true);
	}

	inline char8_t ToLowerCase(char8_t c) {
		if (u8'A' <= c && c <= u8'Z') return c += (u8'a' - u8'A');
		return c;
	}

	inline char8_t ToUpperCase(char8_t c) {
		if (u8'a' <= c && c <= u8'z') return c += (u8'a' - u8'A');
		return c;
	}

	inline bool CompareCaseInsensitive(const char8_t a, const char8_t b) {
		return a == ToLowerCase(b) || a == ToUpperCase(b);
	}
	
	//inline bool IsLetter(const char8_t character)
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

	inline bool IsLetter(const char8_t character) {
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

	inline bool IsWhitespace(const char8_t character) { return character == ' ' || character == '	'; }

	inline bool IsSpecialCharacter(const char8_t character)
	{
		return character == '\n' || character == '\0' || character == '\r' || character == '\f' || character == '\b';
	}
	
	inline bool IsNumber(const char8_t character)
	{
		switch (character) {
		case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
			return true;
		}

		return false;
	}

	inline bool IsSymbol(const char8_t character)
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
