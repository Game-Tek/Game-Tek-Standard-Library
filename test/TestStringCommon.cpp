#include <gtest/gtest.h>

#include "GTSL/ShortString.hpp"
#include "GTSL/StringCommon.h"
#include "GTSL/Unicode.hpp"

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