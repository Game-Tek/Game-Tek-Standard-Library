#include <gtest/gtest.h>

#include "GTSL/ShortString.hpp"

TEST(ShortString, ConstructDefault) {
	GTSL::ShortString<32> string;
	GTEST_ASSERT_EQ(string.GetBytes(), 0);
}

TEST(ShortString, ConstructFromLiteral) {
	GTSL::ShortString<64> string(u8"\U0001F34C \U0001F34E");
	GTEST_ASSERT_EQ(string.GetBytes(), 9);
	GTEST_ASSERT_EQ(string, GTSL::Range(u8"\U0001F34C \U0001F34E"));
}

TEST(ShortString, Append) {
	GTSL::ShortString<32> string(u8"\U0001F34C \U0001F34E");
	string += u8" \U0001F34C \U0001F34E";
	GTEST_ASSERT_EQ(string.GetBytes(), 19);
	GTEST_ASSERT_EQ(string, GTSL::Range(u8"\U0001F34C \U0001F34E \U0001F34C \U0001F34E"));
}