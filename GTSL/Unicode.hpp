#pragma once

#include "Core.h"
#include <array>
#include "Result.h"
#include "Pair.h"
#include <GTSL/Bitman.h>

namespace GTSL {
	struct utf_t {
		char mask;    /* char data will be bitwise AND with this */
		char lead;    /* start bytes of current char in utf-8 encoded character */
		uint32_t beg; /* beginning of codepoint range */
		uint32_t end; /* end of codepoint range */
		int bits_stored; /* the number of bits from the codepoint that fits in char */
	};

	static constexpr utf_t utf[] = {
		/*mask        lead        beg      end       bits */
		{ 0b00111111, 0b10000000, 0,       0,        6 },
		{ 0b01111111, 0b00000000, 0000,    0177,     7 },
		{ 0b00011111, 0b11000000, 0200,    03777,    5 },
		{ 0b00001111, 0b11100000, 04000,   0177777,  4 },
		{ 0b00000111, 0b11110000, 0200000, 04177777, 3 },
		{ 0 }
	};

	inline uint8_t utf8_length(const char8_t buf) {
		constexpr uint8 lengths[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 4, 0 };
		uint8 len = lengths[buf >> 3];
		return { len };
	}

	inline std::array<char8_t, 5> ToUTF8(const char32_t cp)
	{
		std::array<char8_t, 5> ret;
		const int bytes = utf8_length(cp);

		int shift = 6 * (bytes - 1);
		ret[0] = (cp >> shift & utf[bytes].mask) | utf[bytes].lead;
		shift -= 6;
		for (int i = 1; i < bytes; ++i) {
			ret[i] = (cp >> shift & 0b00111111) | 0b10000000;
			shift -= 6;
		}
		ret[bytes] = u8'\0';
		return ret;
	}

	inline char32_t ToUTF32(const char8_t a[]) {
		int bytes = 1;
		int shift = 6/*bits stored*/ * (bytes - 1);
		uint32_t codep = (*a++ & utf[bytes].mask) << shift;

		for (int i = 1; i < bytes; ++i, ++a) {
			shift -= 6/*bits stored*/;
			codep |= ((char)*a & 0b00111111) << shift;
		}

		return (char32_t)codep;
	}

	inline uint8_t utf8_length2(const char8_t* buf) {
		auto bit = 7 - FindLastClearBit((uint8)buf[0]).Get();
		return bit + !bit;
	}

	//https://github.com/skeeto/branchless-utf8
	inline Result<char32_t> utf8_decode(const char8_t* buf, uint8 len)
	{
		constexpr int masks[] = { 0x00, 0x7f, 0x1f, 0x0f, 0x07 };
		constexpr uint32_t mins[] = { 4194304, 0, 128, 2048, 65536 };
		constexpr int shiftc[] = { 0, 18, 12, 6, 0 };


		/* Compute the pointer to the next character early so that the next
		 * iteration can start working on the next character. Neither Clang
		 * nor GCC figure out this reordering on their own.
		 */
		//char8_t* next = s + len + !len;

		uint32_t c = 0;

		/* Assume a four-byte character and load four bytes. Unused bits are
		 * shifted out.
		 */
		c = (uint32_t)(buf[0] & masks[len]) << 18; c |= (uint32_t)(buf[1] & 0x3f) << 12; c |= (uint32_t)(buf[2] & 0x3f) << 6; c |= (uint32_t)(buf[3] & 0x3f) << 0;
		c >>= shiftc[len];

		constexpr int shifte[] = { 0, 6, 4, 2, 0 };

		uint32 e = 0;

		/* Accumulate the various error conditions. */
		e = (c < mins[len]) << 6; // non-canonical encoding
		e |= ((c >> 11) == 0x1b) << 7;  // surrogate half?
		e |= (c > 0x10FFFF) << 8;  // out of range?
		e |= (buf[1] & 0xc0) >> 2;
		e |= (buf[2] & 0xc0) >> 4;
		e |= (buf[3]) >> 6;
		e ^= 0x2a; // top two bits of each tail byte correct?
		e >>= shifte[len];

		return { (char32_t)c, !e };
	}
}