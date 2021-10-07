
#include <gtest/gtest.h>

#include "GTSL/File.h"
#include "GTSL/LUT.hpp"
#include "GTSL/JSON.hpp"
#include "GTSL/Vector.hpp"

TEST(LUT, Valid) {
	GTSL::File lutFile; lutFile.Open(u8"../../../test/Kodak Ektachrome 64.cube", GTSL::File::READ, false);
	GTSL::Buffer<GTSL::DefaultAllocatorReference> buffer;
	lutFile.Read(buffer);

	GTSL::LUTData lutData;

	GTSL::Vector<GTSL::Vector3, GTSL::DefaultAllocatorReference> vectors(33 * 33 * 33);

	auto parseResult = ParseLUT(GTSL::StringView(buffer.GetSize(), buffer.GetSize(), (const char8_t*)buffer.GetData()), lutData, [&](const GTSL::Vector3 vec) { vectors.EmplaceBack(vec); });

	ASSERT_TRUE(parseResult);
	GTEST_ASSERT_EQ(lutData.Size, 33);
	GTEST_ASSERT_EQ(lutData.Min, GTSL::Vector3(0, 0, 0));
	GTEST_ASSERT_EQ(lutData.Max, GTSL::Vector3(1, 1, 1));
	ASSERT_TRUE(GTSL::Math::IsNearlyEqual(vectors[0], GTSL::Vector3(0.0170, 0.0002, 0.0111), 0.0001));
	ASSERT_TRUE(GTSL::Math::IsNearlyEqual(vectors[1], GTSL::Vector3(0.0339, 0.0009, 0.0104), 0.0001));
	ASSERT_TRUE(GTSL::Math::IsNearlyEqual(vectors[2], GTSL::Vector3(0.0519, 0.0015, 0.0097), 0.0001));
}

TEST(LUT, Invalid) {
	auto stringLut = u8" #Created by: Denver Riddle\n #Copyright: (C)Copyright Color Grading Central LLC\n DOMAIN_MIN 0.0 0.0 0.0\nDOMAIN_MAX 0 0 0\nLUT_3D_SIZE 33\n0.0170 0.0002 0.0111\n0.0339 0.0009 0.0104\n0.0519 0.0015 0.0097";

	GTSL::LUTData lut;

	GTSL::StaticVector<GTSL::Vector3, 8> vectors;

	auto parseResult = ParseLUT(stringLut, lut, [&](const GTSL::Vector3 vec) { vectors.EmplaceBack(vec); });

	ASSERT_FALSE(parseResult);
}

TEST(JSON, Serialize) {
	GTSL::uint32 bananas = 5, apples = 2;

	auto jsonString = u8R"({"bananas":5,"apples":2,"string":"test","array":[4,0,8,12],"obj":{"bool":true,"float":3.141000}})";

	GTSL::StaticString<256> buffer;
	GTSL::JSONSerializer serializer = GTSL::MakeSerializer(buffer);

	GTSL::Insert(serializer, buffer, u8"bananas", bananas);
	GTSL::Insert(serializer, buffer, u8"apples", apples);
	GTSL::Insert(serializer, buffer, u8"string", GTSL::StringView(u8"test"));
	GTSL::Insert(serializer, buffer, u8"array", GTSL::Range<const uint32_t*>(GTSL::StaticVector<GTSL::uint32, 4>{ 4, 0, 8, 12 }));

	GTSL::StartObject(serializer, buffer, u8"obj");
		GTSL::Insert(serializer, buffer, u8"bool", true);
		GTSL::Insert(serializer, buffer, u8"float", 3.141f);
	GTSL::EndObject(serializer, buffer);

	EndSerializer(buffer, serializer);

	GTEST_ASSERT_EQ(buffer, jsonString);
}

TEST(JSON, Deserialize) {
	auto jsonString = u8R"({"bananas":5,"apples":2,"string":"test","array":[4,0,8,12],"obj":{"bool":true,"float":3.141}})";

	//todo if key val pattern is not followed error

	GTSL::uint64 bananas = 0, apples = 0;
	GTSL::StaticString<64> string;
	GTSL::StaticVector<GTSL::uint64, 4> array;
	bool boolean = false;
	GTSL::float32 floatingPoint = 0;

	GTSL::JSONDeserializer deserializer;

	auto parseResult = GTSL::Parse(jsonString, deserializer);

	GTSL::uint64 a, b, c, d;

	ASSERT_TRUE(parseResult);

	deserializer(u8"bananas", bananas);
	deserializer(u8"apples", apples);
	deserializer(u8"string", string);
	deserializer(u8"array", array);
	deserializer(u8"array", 0, a);
	deserializer(u8"array", 1, b);
	deserializer(u8"array", 2, c);
	deserializer(u8"array", 3, d);
	deserializer(u8"obj");
	deserializer(u8"bool", boolean);
	deserializer(u8"float", floatingPoint);

	GTEST_ASSERT_EQ(bananas, 5);
	GTEST_ASSERT_EQ(apples, 2);
	GTEST_ASSERT_EQ(string, u8"test");
	GTEST_ASSERT_EQ(array[0], 4);
	GTEST_ASSERT_EQ(array[1], 0);
	GTEST_ASSERT_EQ(array[2], 8);
	GTEST_ASSERT_EQ(array[3], 12);
	GTEST_ASSERT_EQ(a, 4);
	GTEST_ASSERT_EQ(b, 0);
	GTEST_ASSERT_EQ(c, 8);
	GTEST_ASSERT_EQ(d, 12);
	GTEST_ASSERT_EQ(array.GetLength(), 4);
	GTEST_ASSERT_EQ(boolean, true);
	GTEST_ASSERT_EQ(floatingPoint, 3.141f);
}