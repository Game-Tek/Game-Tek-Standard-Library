#pragma once
#include <regex>
#include <GTSL/Core.h>

#include "Range.hpp"
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
	constexpr uint32 StringByteLength(const char8_t* text) noexcept { uint32 i{ 0 }; while (text[i] != '\0') { ++i; } return i; }
	constexpr uint32 StringByteLengthNull(const char8_t* text) noexcept { return StringByteLength(text) + 1; }

	constexpr Pair<uint32, uint32> StringLengths(const char8_t* text) noexcept { 
		uint32 bytes = 0, cp = 0;
		while (text[bytes] != u8'\0') { bytes += UTF8CodePointLength(text[bytes]); ++cp; }
		return { bytes, cp };
	}

	struct StringIterator {
		constexpr StringIterator(const char8_t* d, uint32 b, uint32 cb) : data(d), bytes(b), currentByte(cb) {
		}

		constexpr StringIterator& operator++() {
			currentByte += UTF8CodePointLength(data[currentByte]);
			return *this;
		}

		constexpr StringIterator& operator--() {
			currentByte -= UTF8CodePointLength(data[currentByte]);
			return *this;
		}

		constexpr char32_t operator*() const { return ToUTF32(data + currentByte); }

		constexpr StringIterator operator-(const int32 i) const {
			return StringIterator(data, bytes, currentByte - i);
		}

		constexpr bool operator<(const StringIterator& other) const {
			return currentByte < other.currentByte;
		}

		constexpr bool operator==(const StringIterator& other) const {
			return currentByte == other.currentByte;
		}

		constexpr bool operator!=(const StringIterator& other) const {
			return currentByte != other.currentByte;
		}

		const char8_t* data = nullptr;
		uint32 bytes = 0, currentByte = 0;

		friend class Range<const char8_t*>;
	};

	template<>
	struct Range<const char8_t*>
	{
		constexpr Range() : Range(1, 1, u8"") {}

		constexpr Range(const char8_t* string) : data(string) {
			auto lengths = StringLengths(string);
			bytes = lengths.First; codepoints = lengths.Second;
		}

		template<uint64 N>
		constexpr Range(char8_t const (&string)[N]) : data(string) {
			auto lengths = StringLengths(string);
			bytes = lengths.First; codepoints = lengths.Second;
		}

		constexpr Range(const uint32 bytes, const uint32 codePoints, const char8_t* string) : data(string), codepoints(codePoints), bytes(bytes) {
		}

		constexpr Range(const StringIterator start, const StringIterator end) : data(start.data + start.currentByte) {
			while (start.currentByte + bytes < end.currentByte) { bytes += UTF8CodePointLength(data[bytes]); ++codepoints; }
		}

		[[nodiscard]] constexpr uint32 GetBytes() const { return bytes; }
		[[nodiscard]] constexpr uint32 GetCodepoints() const { return codepoints; }

		[[nodiscard]] constexpr const char8_t* GetData() const { return data; }

		constexpr char8_t operator[](const uint32 index) const { //TODO: FIX
			return data[index];
		}

		[[nodiscard]] constexpr StringIterator begin() const { return StringIterator(data, bytes, 0); }
		[[nodiscard]] constexpr StringIterator end() const { return StringIterator(data, bytes, bytes); }

		constexpr auto operator==(const Range<const char8_t*> other) const {
			if (codepoints != other.codepoints || bytes != other.bytes) { return false; }
			for (uint32 i = 0; i < bytes; ++i) { if (data[i] != other.data[i]) { return false; } }
			return true;
		}

	private:
		const char8_t* data = nullptr; uint32 codepoints = 0, bytes = 0;
	};

	Range(const char8_t*) -> Range<const char8_t*>;

	using StringView = Range<const char8_t*>;

	template<typename...C>
	void ForEachSubstring(const StringView string, auto&& f, C ... divs) {
		static_assert(sizeof...(C) != 0, "You need at least one parameter to divide substrings.");

		auto it = string.begin();

		while (it < string.end()) {
			while (it < string.end() && ((*it == divs) || ...)) {
				++it;
			}

			auto start = it;

			while (it < string.end() and ((*it != divs) && ...)) {
				++it;
			}

			f({ start, it });
		}
	}

	void ForEachLine(const StringView string, auto&& f) {
		ForEachSubstring(string, f, U'\n');
	}

	template<typename...C>
	void Substrings(const StringView string, auto& container, C... divs) {
		static_assert(sizeof...(C) != 0, "You need at least one parameter to divide substrings.");
		auto it = string.begin();

		while (it < string.end() - 2) {
			auto& a = container.EmplaceBack();

			while (it < string.end() and ((*it == divs) || ...)) {
				++it;
			}

			while (it < string.end() and ((*it != divs) && ...)) {
				a += *it;
				++it;
			}
		}
	}

	void Lines(const StringView string, auto& container) {
		Substrings(string, container, U'\n');
	}

	inline bool IsNumber(const StringView string) {
		for (const auto e : string) {
			if (e != U'0' and e != U'1' and e != U'2' and e != U'3' and e != U'4' and e != U'5' and e != U'6' and e != U'7' and e != U'8' and e != U'9' and e != U'.' and e != U',') {
				return false;
			}
		}
		return true;
	}

	inline bool IsDecimalNumber(const StringView string) {
		bool hasDecimalSeparator = false;

		for (const auto e : string) {
			if (e != U'0' and e != U'1' and e != U'2' and e != U'3' and e != U'4' and e != U'5' and e != U'6' and e != U'7' and e != U'8' and e != U'9' and e != U'.') {
				return false;
			}

			if (e == U'.') { hasDecimalSeparator = true; }
		}

		return hasDecimalSeparator;
	}

	template<class S>
	void ToString(S& string, const char8_t* str) {
		string += StringView(str);
	}

	template<class S>
	void ToString(S& string, const Range<const char8_t*> str) {
		string += str;
	}

	template<class S, std::unsigned_integral I>
	void ToString(S& string, I num) {
		uint8 i = 30, len = 0;

		char8_t str[32]; str[31] = u8'\0';

		do {
			// Process individual digits 
			str[i--] = (num % 10) + u8'0';
			num /= 10;
			++len;
		} while (num);

		string += Range<const char8_t*>(len, len, str + i + 1);
	}

	template<class S, std::signed_integral I>
	void ToString(S& string, I num) {
		uint8 i = 30, len = 0;

		char8_t str[32]; str[31] = u8'\0'; bool isNegative = false;

		if (num < 0.0f) { num = -num; isNegative = true; }

		do {
			// Process individual digits 
			str[i--] = (num % 10) + u8'0';
			num /= 10;
			++len;
		} while (num);

		if (isNegative) { str[i--] = u8'-'; ++len; }

		string += GTSL::Range<const char8_t*>(len, len, str + i + 1);
	}

	template<class S>
	void ToString(S& string, float32 num) {
		uint8 i = 30, len = 0;
		char8_t str[32]; str[31] = u8'\0'; bool isNegative = false;

		if (num < 0.0f) { num = -num; isNegative = true; }

		// Extract integer part 
		int32 ipart = static_cast<int32>(num);

		// Extract floating part 
		float32 fpart = num - static_cast<float32>(ipart);

		auto intToStr = [&](int32 integer) {
			do {
				// Process individual digits 
				str[i--] = (integer % 10) + u8'0';
				integer /= 10;
				++len;
			} while (integer);
		};

		// check for display option after point 
		if (6 != 0) {
			// Get the value of fraction part upto given no. 
			// of points after dot. The third parameter  
			// is needed to handle cases like 233.007 
			fpart = fpart * Math::Power(10.0f, 6.0f);

			intToStr(static_cast<int32>(fpart));

			str[i--] = u8'.'; // add dot
			++len;
		}

		// convert integer part to string 
		intToStr(ipart);

		if (isNegative) { str[i--] = u8'-'; ++len; }

		string += GTSL::Range<const char8_t*>(len, len, str + i + 1);
	}
	
	/**
	 * \brief Parses an string to return a number.
	 * \tparam T Type of the number to parse. int, uint, float
	 * \param numberString String containing JUST the number to parse. Tolerates null terminators.
	 * \return Result of the parse. Can fail.
	 */
	template<typename T>
	Result<T> ToNumber(Range<const char8_t*> numberString);

	template<std::unsigned_integral T>
	Result<T> ToNumber(Range<const char8_t*> numberString) {
		uint32 value = 0, mult = 1;

		for (uint64 j = 0, c = numberString.GetBytes() - 1; j < numberString.GetBytes(); ++j, --c) {
			uint8 num;

			switch (numberString[c]) {
			case u8'0': num = 0; break; case u8'1': num = 1; break;
			case u8'2': num = 2; break; case u8'3': num = 3; break;
			case u8'4': num = 4; break; case u8'5': num = 5; break;
			case u8'6': num = 6; break; case u8'7': num = 7; break;
			case u8'8': num = 8; break; case u8'9': num = 9; break;
			case u8'\0': continue;
			default: return Result(0u, false);
			}

			value += num * mult;
			mult *= 10;
		}

		return Result(MoveRef(value), true);
	}

	template<std::signed_integral T>
	Result<T> ToNumber(Range<const char8_t*> numberString) {
		int32 value = 0, mult = 1;

		for (uint64 j = 0, c = numberString.GetBytes() - 1; j < numberString.GetBytes(); ++j, --c) {
			uint8 num;

			switch (numberString[c]) {
			case u8'0': num = 0; break; case u8'1': num = 1; break;
			case u8'2': num = 2; break; case u8'3': num = 3; break;
			case u8'4': num = 4; break; case u8'5': num = 5; break;
			case u8'6': num = 6; break; case u8'7': num = 7; break;
			case u8'8': num = 8; break; case u8'9': num = 9; break;
			case u8'-': num = 0; value = -value; break;
			case u8'\0': continue;
			default: return Result(0, false);
			}

			value += num * mult;
			mult *= 10;
		}

		return Result(MoveRef(value), true);
	}

	template<>
	inline Result<float32> ToNumber(Range<const char8_t*> numberString) {
		float32 value = 0; uint64 c = numberString.GetBytes() - 1/*because of inverse parse*/;

		{
			float32 div = 1.0f;

			while (c <= numberString.GetBytes() - 1) {
				uint8 num;

				switch (numberString[c--]) {
				case u8'0': num = 0; break; case u8'1': num = 1; break;
				case u8'2': num = 2; break; case u8'3': num = 3; break;
				case u8'4': num = 4; break; case u8'5': num = 5; break;
				case u8'6': num = 6; break; case u8'7': num = 7; break;
				case u8'8': num = 8; break; case u8'9': num = 9; break;
				case u8'.': {
					value /= div;
					num = 0; div = 0.1f;
					break;
				}
				case u8'\0': continue;
				case u8'-': {
					value *= -1; num = 0;
					break;
				}
				default: return Result<float32>(0, false);
				}

				value += num * div;
				div *= 10;
			}
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

	inline bool IsLetter(const char8_t character) {
		switch (character) {
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

	inline bool IsSpecialCharacter(const char8_t character) {
		return character == '\n' || character == '\0' || character == '\r' || character == '\f' || character == '\b';
	}
	
	inline bool IsNumber(const char8_t character) {
		switch (character) {
		case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
			return true;
		}

		return false;
	}

	inline bool IsSymbol(const char8_t character) {
		switch (character) {
		case '!': case '\"': case '#': case '|': case '\'': case '$': case '%': case '&': case '/': case '(': case ')':
		case '=': case '?': case '[': case ']': case '^': case '*': case '{': case '}': case ',': case '.': case ';':
		case '<': case '>': case '_': case '~': case '-': case '+':
			return true;
		}

		return false;
	}
}
