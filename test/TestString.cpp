#include <gtest/gtest.h>

#include "GTSL/String.hpp"

using namespace GTSL;

TEST(String, Construct) {
	String<DefaultAllocatorReference> string(u8"\U0001F34C \U0001F34E");
	GTEST_ASSERT_EQ(string.GetCodepoints(), 4);
	GTEST_ASSERT_EQ(string.GetBytes(), 10);

	string += u8't';

	GTEST_ASSERT_EQ(string.GetCodepoints(), 5);
	GTEST_ASSERT_EQ(string.GetBytes(), 11);

	string += u8"\U0001f975";

	GTEST_ASSERT_EQ(string.GetCodepoints(), 6);
	GTEST_ASSERT_EQ(string.GetBytes(), 15);	

	string += u8"\U0001f4A9 top";

	GTEST_ASSERT_EQ(string.GetCodepoints(), 11);
	GTEST_ASSERT_EQ(string.GetBytes(), 23);
}

TEST(String, Resize) {
	auto testString = u8"abcdefghijklmnopqrstuvwxyz0123456789";

	String<DefaultAllocatorReference> string(testString);

	auto bytes = string.GetBytes();
	auto codePoints = string.GetCodepoints();
	auto capacity = string.GetCapacity();

	string.Resize(capacity + 8);

	GTEST_ASSERT_EQ(bytes, string.GetBytes());
	GTEST_ASSERT_EQ(codePoints, string.GetCodepoints());
	GTEST_ASSERT_GT(string.GetCapacity(), capacity);

	for(uint32_t i = 0; i < 36; ++i) {
		GTEST_ASSERT_EQ(string[i], ToUTF32(testString + i));
	}
}

TEST(String, Append) {
	String<DefaultAllocatorReference> string(u8"\U0001F34C");
	string += u8"\U0001f975";

	GTEST_ASSERT_EQ(string.GetCodepoints(), 3);
	GTEST_ASSERT_EQ(string.GetBytes(), 9);

	string += u8"foo";

	GTEST_ASSERT_EQ(string.GetCodepoints(), 6);
	GTEST_ASSERT_EQ(string.GetBytes(), 12);
}

TEST(String, Drop) {
	String<DefaultAllocatorReference> string(u8"Drop \U0001F34C");
	string.Drop(5);

	GTEST_ASSERT_EQ(string.GetCodepoints(), 6);
	GTEST_ASSERT_EQ(string.GetBytes(), 6);
}