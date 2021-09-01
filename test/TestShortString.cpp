#include <gtest/gtest.h>

#include "GTSL/ShortString.hpp"

TEST(ShortString, Construct) {
	GTSL::ShortString<32> string(u8"\U0001F34C \U0001F34E");
	GTEST_ASSERT_EQ(string.GetLength(), 10);
}

TEST(ShortString, Append) {
	GTSL::ShortString<32> string(u8"\U0001F34C \U0001F34E");
	string += u8" \U0001F34C \U0001F34E";
	GTEST_ASSERT_EQ(string.GetLength(), 20);
}