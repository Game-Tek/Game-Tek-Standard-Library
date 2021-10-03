
#include <gtest/gtest.h>

#include "GTSL/File.h"
#include "GTSL/LUT.hpp"
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