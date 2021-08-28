#include <gtest/gtest.h>

#include "GTSL/String.hpp"

using namespace GTSL;

TEST(UTF8, Parse) {
	GTEST_ASSERT_EQ(StringByteLength(u8"Test the length of the string."), 30 + 1);
	GTEST_ASSERT_EQ(StringLengths(u8"Test the length of the string. \U0001f975").First, 35);
	GTEST_ASSERT_EQ(StringLengths(u8"Test the length of the string. \U0001f975").Second, 32);
}

TEST(String, Construct) {
	String<DefaultAllocatorReference> string(u8"\U0001F34C \U0001F34E");
	GTEST_ASSERT_EQ(string.GetCodepoints(), 3);
	GTEST_ASSERT_EQ(string.GetBytes(), 9);

	string += u8't';

	GTEST_ASSERT_EQ(string.GetCodepoints(), 4);
	GTEST_ASSERT_EQ(string.GetBytes(), 10);

	string += u8"\U0001f975";

	GTEST_ASSERT_EQ(string.GetCodepoints(), 5);
	GTEST_ASSERT_EQ(string.GetBytes(), 14);	

	string += u8"\U0001f4A9 top";

	GTEST_ASSERT_EQ(string.GetCodepoints(), 10);
	GTEST_ASSERT_EQ(string.GetBytes(), 22);
}