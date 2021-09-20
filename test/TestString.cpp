#include <gtest/gtest.h>

#include "GTSL/String.hpp"

using namespace GTSL;

TEST(String, ConstructFromSize) {
	String<DefaultAllocatorReference> string(32);
	GTEST_ASSERT_EQ(string.GetCodepoints(), 1);
	GTEST_ASSERT_EQ(string.GetBytes(), 1);
	GTEST_ASSERT_GE(string.GetCapacity(), 32);
}

TEST(String, ConstructFromString) {
	String<DefaultAllocatorReference> string(u8"\U0001F34C \U0001F34E");
	GTEST_ASSERT_EQ(string.GetCodepoints(), 4);
	GTEST_ASSERT_EQ(string.GetBytes(), 10);
	GTEST_ASSERT_GE(string.GetCapacity(), 10);
}

TEST(String, CopyConstruct) {
	auto testString = u8"\U0001F34C test \U0001F34E";

	String<DefaultAllocatorReference> string0(testString);

	String string1(static_cast<String<DefaultAllocatorReference>&>(string0)); //force copy constructor

	//test original string hasn't been modified
	GTEST_ASSERT_EQ(string0.GetBytes(), StringLengths(testString).First);
	GTEST_ASSERT_EQ(string0.GetCodepoints(), StringLengths(testString).Second);

	GTEST_ASSERT_EQ(string1.GetCodepoints(), string0.GetCodepoints());
	GTEST_ASSERT_EQ(string1.GetBytes(), string0.GetBytes());

	for(uint32 i = 0; i < StringByteLength(testString); ++i) {
		GTEST_ASSERT_EQ(string0.c_str()[i], testString[i]); //test original string hasn't been modified
		GTEST_ASSERT_EQ(string0.c_str()[i], string1.c_str()[i]);
	}
}

TEST(String, MoveConstruct) {
	auto testString = u8"\U0001F34C test \U0001F34E";

	String<DefaultAllocatorReference> string0(testString);

	auto string0Bytes = string0.GetBytes(); auto string0Codepoints = string0.GetCodepoints(); auto string0pointer = string0.c_str();

	String string1(static_cast<String<DefaultAllocatorReference>&&>(string0)); //force move constructor

	//test original string have been modified
	GTEST_ASSERT_EQ(string0.GetBytes(), 1/*null terminator*/); GTEST_ASSERT_EQ(string0.GetCodepoints(), 1/*null terminator*/); GTEST_ASSERT_EQ(string0.c_str(), nullptr); GTEST_ASSERT_EQ(string0.IsEmpty(), true);

	//test information has been moved
	GTEST_ASSERT_EQ(string1.GetCodepoints(), string0Codepoints); GTEST_ASSERT_EQ(string1.GetBytes(), string0Bytes); GTEST_ASSERT_EQ(string1.c_str(), string0pointer); //remember static allocators DO switch pointers so this may not be a necessarily true test

	for (uint32 i = 0; i < StringByteLength(testString); ++i) {
		GTEST_ASSERT_EQ(string1.c_str()[i], testString[i]); //test string has been moved correctly
	}
}

TEST(String, CopyAssignment) {
	auto testString = u8"\U0001F34C test \U0001F34E";

	String<DefaultAllocatorReference> string0(testString);

	String<DefaultAllocatorReference> string1;
	string1 = string0; //force copy constructor //TODO: TEST CODE EMITTED

	//test original string hasn't been modified
	GTEST_ASSERT_EQ(string0.GetBytes(), StringLengths(testString).First);
	GTEST_ASSERT_EQ(string0.GetCodepoints(), StringLengths(testString).Second);

	GTEST_ASSERT_EQ(string1.GetCodepoints(), string0.GetCodepoints());
	GTEST_ASSERT_EQ(string1.GetBytes(), string0.GetBytes());

	for (uint32 i = 0; i < StringByteLength(testString); ++i) {
		GTEST_ASSERT_EQ(string0.c_str()[i], testString[i]); //test original string hasn't been modified
		GTEST_ASSERT_EQ(string0.c_str()[i], string1.c_str()[i]); //use begin() to access bytes, not codepoints
	}
}

TEST(String, MoveAssignment) {
	auto testString = u8"\U0001F34C test \U0001F34E";

	String<DefaultAllocatorReference> string0(testString);

	auto string0Bytes = string0.GetBytes(); auto string0Codepoints = string0.GetCodepoints(); auto string0pointer = string0.c_str();

	String<DefaultAllocatorReference> string1;
	string1 = static_cast<String<DefaultAllocatorReference>&&>(string0);

	//test original string have been modified
	GTEST_ASSERT_EQ(string0.GetBytes(), 1/*null terminator*/); GTEST_ASSERT_EQ(string0.GetCodepoints(), 1/*null terminator*/); GTEST_ASSERT_EQ(string0.c_str(), nullptr); GTEST_ASSERT_EQ(string0.IsEmpty(), true);

	//test information has been moved
	GTEST_ASSERT_EQ(string1.GetCodepoints(), string0Codepoints); GTEST_ASSERT_EQ(string1.GetBytes(), string0Bytes); GTEST_ASSERT_EQ(string1.c_str(), string0pointer); //remember static allocators DO switch pointers so this may not be a necessarily true test

	for (uint32 i = 0; i < StringByteLength(testString); ++i) {
		GTEST_ASSERT_EQ(string1.c_str()[i], testString[i]); //test string has been moved correctly
	}
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

	for(uint32_t i = 0; i < StringByteLength(testString); ++i) {
		GTEST_ASSERT_EQ(string.c_str()[i], testString[i]);
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

TEST(String, FindFirst) {
	String<DefaultAllocatorReference> string(u8"abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789");

	auto aSearch = FindFirst(string, u8'a');

	ASSERT_TRUE(aSearch.State());
	GTEST_ASSERT_EQ(aSearch.Get(), 0);

	auto zeroSearch = FindFirst(string, u8'0');

	ASSERT_TRUE(zeroSearch.State());
	GTEST_ASSERT_EQ(zeroSearch.Get(), 26);

	auto nullSearch = FindFirst(string, u8'\0');

	ASSERT_TRUE(nullSearch.State());
	GTEST_ASSERT_EQ(nullSearch.Get(), 72);

	auto invalidSearch = FindFirst(string, u8'!');
	ASSERT_FALSE(invalidSearch.State());
}

TEST(String, FindLast) {
	String<DefaultAllocatorReference> string(u8"abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789");

	auto aSearch = FindLast(string, u8'a');

	ASSERT_TRUE(aSearch.State());
	GTEST_ASSERT_EQ(aSearch.Get(), 36);

	auto zeroSearch = FindLast(string, u8'0');

	ASSERT_TRUE(zeroSearch.State());
	GTEST_ASSERT_EQ(zeroSearch.Get(), 62);

	auto nullSearch = FindLast(string, u8'\0');

	ASSERT_TRUE(nullSearch.State());
	GTEST_ASSERT_EQ(nullSearch.Get(), 72);

	auto invalidSearch = FindLast(string, u8'!');
	ASSERT_FALSE(invalidSearch.State());
}

TEST(String, Iterator) {
	auto testString = u8"abcdefghijklmnopqrstuvwxyz0123456789";

	String<DefaultAllocatorReference> string(testString);

	auto begin = string.begin(); uint32 i = 0;

	while(true) {
		if (begin == string.end()) { break; }
		GTEST_ASSERT_EQ(*begin, ToUTF32(testString + i++));
		++begin;
	}
}