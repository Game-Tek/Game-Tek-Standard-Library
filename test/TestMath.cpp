#include <gtest/gtest.h>

#include "GTSL/Math/Math.hpp"

using namespace GTSL;

TEST(Math, Sines) {
	EXPECT_FLOAT_EQ(Math::sin(0.0f), 0.0f);
	EXPECT_FLOAT_EQ(Math::sin((-GTSL::Math::PI / 4) * 10), -1.0f);
	EXPECT_FLOAT_EQ(Math::sin((-GTSL::Math::PI / 2) * 10), 0);
	EXPECT_FLOAT_EQ(Math::sin(-GTSL::Math::PI / 4), -0.7071067f);
	EXPECT_FLOAT_EQ(Math::sin(-GTSL::Math::PI / 2), -1.0f);
	EXPECT_FLOAT_EQ(Math::sin(GTSL::Math::PI), 0.0f);
	EXPECT_FLOAT_EQ(Math::sin(GTSL::Math::PI / 2), 1.0f);
	EXPECT_FLOAT_EQ(Math::sin(GTSL::Math::PI / 4), 0.7071067f);
	EXPECT_FLOAT_EQ(Math::sin((GTSL::Math::PI / 2) * 10), 0);
	EXPECT_FLOAT_EQ(Math::sin((GTSL::Math::PI / 4) * 10), 1.0f);
}

TEST(Math, Cosines) {
}

//TEST(Math, Vector2D) {
//	const Vector2 vector(1, -1);
//
//	EXPECT_FLOAT_EQ(vector[0], 1.0f);
//	EXPECT_FLOAT_EQ(vector.X(), 1.0f);
//
//	EXPECT_FLOAT_EQ(vector[1], -1.0f);
//	EXPECT_FLOAT_EQ(vector.Y(), -1.0f);
//
//	{
//		auto vector2 = vector;
//		EXPECT_EQ(vector2 + 2, GTSL::Vector2(3, 1));
//	}
//
//	{
//		auto vector2 = vector;
//		EXPECT_EQ(vector2 - 2, GTSL::Vector2(-1, -3));
//	}
//
//	{
//		auto vector2 = vector;
//		EXPECT_EQ(vector2 * 2, GTSL::Vector2(2, -2));
//	}
//
//	{
//		auto vector2 = vector;
//		EXPECT_EQ(vector2 / 2, GTSL::Vector2(0.5f, -0.5f));
//	}
//
//	{
//		auto vector2 = vector;
//		vector2 += 2;
//		EXPECT_EQ(vector2, GTSL::Vector2(3, -1));
//	}
//
//	{
//		auto vector2 = vector;
//		vector2 -= 2;
//		EXPECT_EQ(vector2, GTSL::Vector2(-1, -3));
//	}
//
//	{
//		auto vector2 = vector;
//		vector2 *= 2;
//		EXPECT_EQ(vector2, GTSL::Vector2(2, -2));
//	}
//
//	{
//		auto vector2 = vector;
//		vector2 /= 2;
//		EXPECT_EQ(vector2, GTSL::Vector2(0.5f, -0.5f));
//	}
//}

TEST(Math, Vector3D) {
}

TEST(Math, Vector4D) {
}

TEST(Math, Matrix4x4) {
	Matrix4 matrix( 1, 2, 3, 4,
					5, 1, 6, 7,
					8, 9, 1, 10,
					11, 12, 13, 1);

	EXPECT_FLOAT_EQ(matrix[0][0], 1); EXPECT_FLOAT_EQ(matrix[1][1], 1); EXPECT_FLOAT_EQ(matrix[2][2], 1); EXPECT_FLOAT_EQ(matrix[3][3], 1);

	//EXPECT_EQ(matrix * 2, Matrix4());
}