#pragma once
#include <regex>

#include "Core.h"

#include "DataSizes.h"
#include "Result.h"
#include "Tuple.hpp"
#include "Math/Math.hpp"
#include "Unicode.hpp"
#include "Algorithm.hpp"

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

		constexpr StringIterator& operator++(int) {
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
	struct Range<const char8_t*> {
		constexpr Range() = default;

		constexpr Range(const char8_t* string) : data(string) {
			auto lengths = StringLengths(string);
			bytes = lengths.First; codepoints = lengths.Second;
		}

		template<uint64 N>
		constexpr Range(char8_t const (&string)[N]) : data(string) {
			auto lengths = StringLengths(string);
			bytes = lengths.First; codepoints = lengths.Second;
		}

		constexpr Range(const Byte bytes, const char8_t* string) : data(string), bytes(bytes) {
			for (uint32 i = 0; i < bytes.GetCount();) { i += UTF8CodePointLength(data[i]); ++codepoints; }
		}

		constexpr Range(const uint32 bytes, const uint32 codePoints, const char8_t* string) : data(string), codepoints(codePoints), bytes(bytes) {
		}

		constexpr Range(const StringIterator start, const StringIterator end) : data(start.data + start.currentByte) {
			while (start.currentByte + bytes < end.currentByte) { bytes += UTF8CodePointLength(data[bytes]); ++codepoints; }
		}

		[[nodiscard]] constexpr uint32 GetBytes() const { return bytes; }
		[[nodiscard]] constexpr uint32 GetCodepoints() const { return codepoints; }

		[[nodiscard]] constexpr const char8_t* GetData() const { return data; }

		constexpr char32_t operator[](const uint32 codepointIndex) const {
			uint32 byte = 0, codepoint = 0;
			while (codepoint != codepointIndex) { byte += UTF8CodePointLength(data[byte]); ++codepoint; }
			return ToUTF32(data + byte);
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

	template<>
	struct Hash<StringView> {
		uint64 value = 0;

		constexpr Hash(const StringView& string_view) {
			uint64 primary_hash(525201411107845655ull);

			for (uint32 i = 0; i < string_view.GetBytes(); ++i) {
				primary_hash ^= string_view[i]; primary_hash *= 0x5bd1e9955bd1e995; primary_hash ^= primary_hash >> 47;
			}

			value = primary_hash;
		}

		constexpr operator uint64() const { return value; }
	};

	Hash(const StringView) -> Hash<StringView>;

	class SubstringIterator {
	public:
		SubstringIterator(const StringView sv) : stringView(sv), begin(sv.begin()), end(sv.begin()) {}

		template<typename...C>
		bool operator()(C ... divs) {

			while (end < stringView.end() && ((*end == divs) || ...)) {
				++end;
			}

			begin = end;

			while (end < stringView.end() and ((*end != divs) && ...)) {
				++end;
			}

			return begin != end;
		}

		StringView operator*() {
			return StringView(begin, end);
		}

	private:
		StringView stringView;
		StringIterator begin, end;

		//static_assert(sizeof...(C) != 0, "You need at least one parameter to divide substrings.");
	};

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
			if (e != U'0' and e != U'1' and e != U'2' and e != U'3' and e != U'4' and e != U'5' and e != U'6' and e != U'7' and e != U'8' and e != U'9' and e != U'.' and e != U',' and e != U'-') {
				return false;
			}
		}
		return string.GetBytes();
	}

	inline bool IsDecimalNumber(const StringView string) {
		bool hasDecimalSeparator = false;

		for (const auto e : string) {
			if (e != U'0' and e != U'1' and e != U'2' and e != U'3' and e != U'4' and e != U'5' and e != U'6' and e != U'7' and e != U'8' and e != U'9' and e != U'.' and e != U'-') {
				return false;
			}

			if (e == U'.') { hasDecimalSeparator = true; }
		}

		return hasDecimalSeparator;
	}

	template<class S>
	void ToString(S& string, char8_t str) {
		string += str;
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
		T value = 0, mult = 1;

		for (uint64 j = 0, c = numberString.GetBytes() - 1; j < numberString.GetBytes(); ++j, --c) {
			uint8 num;

			switch (numberString[c]) {
			case u8'0': num = 0; break; case u8'1': num = 1; break;
			case u8'2': num = 2; break; case u8'3': num = 3; break;
			case u8'4': num = 4; break; case u8'5': num = 5; break;
			case u8'6': num = 6; break; case u8'7': num = 7; break;
			case u8'8': num = 8; break; case u8'9': num = 9; break;
			case u8'\0': continue;
			default: return Result(static_cast<T>(0), false);
			}

			value += num * mult;
			mult *= 10;
		}

		return Result(MoveRef(value), true);
	}

	template<std::signed_integral T>
	Result<T> ToNumber(Range<const char8_t*> numberString) {
		T value = 0, mult = 1;

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
			default: return Result(static_cast<T>(0), false);
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

	inline bool IsLetter(const char32_t character) {
		constexpr char32_t table[] = { U'A', U'B', U'C', U'D', U'E', U'F', U'G', U'H', U'I', U'J', U'K', U'L', U'M', U'N', U'O', U'P', U'Q', U'R', U'S', U'T', U'U', U'V', U'W', U'X', U'Y', U'Z',
			                           U'a', U'b', U'c', U'd', U'e', U'f', U'g', U'h', U'i', U'j', U'k', U'l', U'm', U'n', U'o', U'p', U'q', U'r', U's', U't', U'u', U'v', U'w', U'x', U'y', U'z'};

		for (auto& e : table) { if (e == character) { return true; } }

		return false;
	}

	inline bool IsWhitespace(const char32_t character) { return character == U' ' || character == U'	'; }

	inline bool IsSpecialCharacter(const char32_t character) {
		return character == U'\n' || character == U'\0' || character == U'\r' || character == U'\f' || character == U'\b';
	}
	
	inline bool IsNumber(const char32_t character) {
		switch (character) {
		case U'0': case U'1': case U'2': case U'3': case U'4': case U'5': case U'6': case U'7': case U'8': case U'9':
			return true;
		}

		return false;
	}

	inline bool IsSymbol(const char32_t character) {
		switch (character) {
		case U'!': case U'\"': case U'#': case U'|': case U'\'': case U'$': case U'%': case U'&': case U'/': case U'(': case U')':
		case U'=': case U'?':  case U'[': case U']': case U'^':  case U'*': case U'{': case U'}': case U',': case U'.': case U';': case U':':
		case U'<': case U'>':  case U'_': case U'~': case U'-':  case U'+':
			return true;
		}

		return false;
	}

	struct Join {
		GTSL::Range<const GTSL::StringView*> Strings;
		GTSL::StringView Connector;
	};
}
