#include <gtest/gtest.h>

#include "GTSL/ShortString.hpp"
#include "GTSL/String.hpp"
#include "GTSL/StringCommon.h"
#include "GTSL/Unicode.hpp"
#include "GTSL/Vector.hpp"

using namespace GTSL;

TEST(StringCommon, NumberToString) {
	{
		GTSL::ShortString<32> string;
		ToString(string, 32u);
		GTEST_ASSERT_EQ(string, Range(u8"32"));
	}

	{
		GTSL::ShortString<32> string;
		ToString(string, -28);
		GTEST_ASSERT_EQ(string, Range(u8"-28"));
	}

	{
		GTSL::ShortString<32> string;
		ToString(string, 0);
		GTEST_ASSERT_EQ(string, Range(u8"0"));
	}

	{
		GTSL::ShortString<32> string;
		ToString(string, 3.141f);
		GTEST_ASSERT_EQ(string, Range(u8"3.141000"));
	}
}

TEST(StringCommon, StringToNumber) {
	{
		ShortString<32> string(u8"3.141");
		auto number0 = ToNumber<float32>(string);
		GTEST_ASSERT_EQ(number0.State(), true);
		ASSERT_FLOAT_EQ(number0.Get(), 3.141f);
	}

	{
		ShortString<32> string(u8"4");
		auto number0 = ToNumber<float32>(string);
		GTEST_ASSERT_EQ(number0.State(), true);
		ASSERT_FLOAT_EQ(number0.Get(), 4.f);
	}

	{
		ShortString<32> string(u8"-3.141");
		auto number0 = ToNumber<float32>(string);
		GTEST_ASSERT_EQ(number0.State(), true);
		ASSERT_FLOAT_EQ(number0.Get(), -3.141f);
	}

	{
		ShortString<32> string(u8"-8");
		auto number0 = ToNumber<float32>(string);
		GTEST_ASSERT_EQ(number0.State(), true);
		ASSERT_FLOAT_EQ(number0.Get(), -8.f);
	}

	{
		ShortString<32> string(u8"-8");
		auto number0 = ToNumber<int32>(string);
		GTEST_ASSERT_EQ(number0.State(), true);
		ASSERT_FLOAT_EQ(number0.Get(), -8);
	}

	{
		ShortString<32> string(u8"-8");
		auto number0 = ToNumber<uint32>(string);
		GTEST_ASSERT_EQ(number0.State(), false);
	}

	{
		ShortString<32> string(u8"-8 ay32.");
		auto number0 = ToNumber<int32>(string);
		GTEST_ASSERT_EQ(number0.State(), false);
	}

	{
		ShortString<32> string(u8"a.58 -8");
		auto number0 = ToNumber<int32>(string);
		GTEST_ASSERT_EQ(number0.State(), false);
	}
}

TEST(StringCommon, ForEachSubstring) {
	StaticString<64> string(u8"a bcd efghi  j");

	uint32 i = 0;

	ForEachSubstring(string, [&](StringView substring) {
		switch (i) {
		case 0:
			GTEST_ASSERT_EQ(substring, u8"a");
			break;
		case 1:
			GTEST_ASSERT_EQ(substring, u8"bcd");
			break;
		case 2:
			GTEST_ASSERT_EQ(substring, u8"efghi");
			break;
		case 3:
			GTEST_ASSERT_EQ(substring, u8"j");
			break;
		}

		++i;
	}, U' ');

	StaticVector<StaticString<16>, 4> vector;

	Substrings(string, vector, U' ');
	GTEST_ASSERT_EQ(vector[0], u8"a");
	GTEST_ASSERT_EQ(vector[1], u8"bcd");
	GTEST_ASSERT_EQ(vector[2], u8"efghi");
	GTEST_ASSERT_EQ(vector[3], u8"j");
}
