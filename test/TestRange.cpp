#include <gtest/gtest.h>

#include "GTSL/StringCommon.h"
#include "GTSL/Unicode.hpp"

using namespace GTSL;

TEST(Range, StringView) {
	const auto a = Range(u8"Build a range.");

	GTEST_ASSERT_EQ(a.GetBytes(), 14);
	GTEST_ASSERT_EQ(a.GetCodepoints(), 14);

	GTEST_ASSERT_EQ(Range(u8"These strings are the same."), Range(u8"These strings are the same."));
	GTEST_ASSERT_NE(Range(u8"These strings aren't the same."), Range(u8"These strings AREN'T the same."));
}
