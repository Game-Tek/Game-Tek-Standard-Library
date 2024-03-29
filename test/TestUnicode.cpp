#include <gtest/gtest.h>

#include "GTSL/StringCommon.h"
#include "GTSL/Unicode.hpp"

using namespace GTSL;

TEST(Unicode, StringLength) {
	GTEST_ASSERT_EQ(StringByteLength(u8"Test the length of the string. \U0001f975"), 35);
	GTEST_ASSERT_EQ(StringLengths(u8"Test the length of the string. \U0001f975").First, 35);  //bytes
	GTEST_ASSERT_EQ(StringLengths(u8"Test the length of the string. \U0001f975").Second, 32); //codepoints
}

TEST(Unicode, CodepointLength) {
	GTEST_ASSERT_EQ(UTF8CodePointLength(0b11110000), 4);
	GTEST_ASSERT_EQ(UTF8CodePointLength(0b11100000), 3);
	GTEST_ASSERT_EQ(UTF8CodePointLength(0b11000000), 2);
	GTEST_ASSERT_EQ(UTF8CodePointLength(0b01000000), 1);
	GTEST_ASSERT_EQ(UTF8CodePointLength(0b10011111), 0);
}

TEST(Unicode, CharConversion) {
	auto string1 = u8"\U0000006A";
	GTEST_ASSERT_EQ(ToUTF32(string1), U'\U0000006A');
	GTEST_ASSERT_EQ(ToUTF32(string1[0], string1[1], 0, 0, 1).Get(), U'\U0000006A');

	auto string2 = u8"\U000000F1";
	GTEST_ASSERT_EQ(ToUTF32(string2), U'\U000000F1');
	GTEST_ASSERT_EQ(ToUTF32(string2[0], string2[1], string2[2], 0, 2).Get(), U'\U000000F1');

	auto string3 = u8"\U00002699";
	GTEST_ASSERT_EQ(ToUTF32(string3), U'\U00002699');
	GTEST_ASSERT_EQ(ToUTF32(string3[0], string3[1], string3[2], string3[3], 3).Get(), U'\U00002699');

	auto string4 = u8"\U0001F9F0";
	GTEST_ASSERT_EQ(ToUTF32(string4), U'\U0001F9F0');
	GTEST_ASSERT_EQ(ToUTF32(string4[0], string4[1], string4[2], string4[3], 4).Get(), U'\U0001F9F0');
}

TEST(Unicode, Malformed) {
	char8_t badUTF8[] = { 0xF0, 0xF9, 0x8D, 0x00 /*, 0x8C*/ };
	ASSERT_EQ(ToUTF32(badUTF8[0], badUTF8[1], badUTF8[2], badUTF8[3], 4).State(), false);

	char8_t UTF16[] = { 0xD8, 0x3C, 0xDF, 0x4C };
	ASSERT_EQ(ToUTF32(UTF16[0], UTF16[1], UTF16[2], UTF16[3], 4).State(), false);

	char8_t UTF32[] = { 0x00, 0x01, 0xF3, 0x4C };
	ASSERT_EQ(ToUTF32(UTF32[0], UTF32[1], UTF32[2], UTF32[3], 4).State(), false);
}

TEST(Unicode, Wellformed) {
	char8_t good[] = { 0b11110000, 0b10011111 ,0b10001101 ,0b10001100 };
	ASSERT_EQ(ToUTF32(good[0], good[1], good[2], good[3], 4).State(), true);
}