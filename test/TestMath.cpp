#include <gtest/gtest.h>

#include "GTSL/Math/Math.hpp"

using namespace GTSL;

TEST(Math, Sines) {
	EXPECT_FLOAT_EQ(0.0f,			Math::sin(0.0f));

	EXPECT_FLOAT_EQ(-1.0f,			Math::sin(-GTSL::Math::PI / 4 * 10));
	EXPECT_NEAR(0.0f,				Math::sin(-GTSL::Math::PI / 2 * 10), 0.00001f);

	EXPECT_FLOAT_EQ(-0.7071067f,	Math::sin(-GTSL::Math::PI / 4));
	EXPECT_FLOAT_EQ(-1.0f,			Math::sin(-GTSL::Math::PI / 2));

	EXPECT_NEAR(0.0f,				Math::sin(GTSL::Math::PI), 0.00001f);

	EXPECT_FLOAT_EQ(1.0f,			Math::sin(GTSL::Math::PI / 2));
	EXPECT_FLOAT_EQ(0.7071067f,		Math::sin(GTSL::Math::PI / 4));

	EXPECT_NEAR(0.0f,				Math::sin(GTSL::Math::PI / 2 * 10), 0.00001f);
	EXPECT_FLOAT_EQ(1.0f,			Math::sin(GTSL::Math::PI / 4 * 10));
}

TEST(Math, Cosines) {
	EXPECT_FLOAT_EQ(1.0f, Math::cos(0.0f));

	EXPECT_NEAR(0.0f, Math::cos(-GTSL::Math::PI / 4 * 10), 0.00001f);
	EXPECT_FLOAT_EQ(-1.0f, Math::cos(-GTSL::Math::PI / 2 * 10));

	EXPECT_FLOAT_EQ(0.7071067f, Math::cos(-GTSL::Math::PI / 4));
	EXPECT_NEAR(0.0f, Math::cos(-GTSL::Math::PI / 2), 0.00001f);

	EXPECT_NEAR(-1.0f, Math::cos(GTSL::Math::PI), 0.00001f);

	EXPECT_NEAR(0.0f, Math::cos(GTSL::Math::PI / 2), 0.00001f);
	EXPECT_FLOAT_EQ(0.7071067f, Math::cos(GTSL::Math::PI / 4));

	EXPECT_FLOAT_EQ(-1.0f, Math::cos(GTSL::Math::PI / 2 * 10));
	EXPECT_NEAR(0.0f, Math::cos(GTSL::Math::PI / 4 * 10), 0.00001f);
}

TEST(Math, Vector2D) {
	const Vector2 vector(1, -1);

	EXPECT_FLOAT_EQ(vector[0], 1.0f);
	EXPECT_FLOAT_EQ(vector.X(), 1.0f);

	EXPECT_FLOAT_EQ(vector[1], -1.0f);
	EXPECT_FLOAT_EQ(vector.Y(), -1.0f);

	{
		auto vector2 = vector + 2;
		GTEST_ASSERT_EQ(vector2.X(), 3); GTEST_ASSERT_EQ(vector2.Y(), 1);
	}

	{
		auto vector2 = vector - 2;
		GTEST_ASSERT_EQ(vector2.X(), -1); GTEST_ASSERT_EQ(vector2.Y(), -3);
	}
	
	{
		auto vector2 = vector * 2;
		GTEST_ASSERT_EQ(vector2.X(), 2); GTEST_ASSERT_EQ(vector2.Y(), -2);
	}
	
	{
		auto vector2 = vector / 2;
		GTEST_ASSERT_EQ(vector2.X(), 0.5); GTEST_ASSERT_EQ(vector2.Y(), -0.5f);
	}
	
	{
		auto vector2 = vector;
		vector2 += 2;
		ASSERT_TRUE(vector2 == GTSL::Vector2(3, 1));
	}
	
	{
		auto vector2 = vector;
		vector2 -= 2;
		ASSERT_TRUE(vector2 == GTSL::Vector2(-1, -3));
	}
	
	{
		auto vector2 = vector;
		vector2 *= 2;
		ASSERT_TRUE(vector2 == GTSL::Vector2(2, -2));
	}
	
	{
		auto vector2 = vector;
		vector2 /= 2;
		ASSERT_TRUE(vector2 == GTSL::Vector2(0.5f, -0.5f));
	}
}

//TEST(Math, Vector3D) {
//}
//
//TEST(Math, Vector4D) {
//}

TEST(Math, MatrixConstruct) {
	Matrix4 matrix( 1, 2, 3, 4,
					5, 1, 6, 7,
					8, 9, 1, 10,
					11, 12, 13, 1);

	EXPECT_FLOAT_EQ(matrix[0][0], 1); EXPECT_FLOAT_EQ(matrix[0][1], 2); EXPECT_FLOAT_EQ(matrix[0][2], 3); EXPECT_FLOAT_EQ(matrix[0][3], 4);
	EXPECT_FLOAT_EQ(matrix[1][0], 5); EXPECT_FLOAT_EQ(matrix[1][1], 1); EXPECT_FLOAT_EQ(matrix[1][2], 6); EXPECT_FLOAT_EQ(matrix[1][3], 7);
	EXPECT_FLOAT_EQ(matrix[2][0], 8); EXPECT_FLOAT_EQ(matrix[2][1], 9); EXPECT_FLOAT_EQ(matrix[2][2], 1); EXPECT_FLOAT_EQ(matrix[2][3], 10);
	EXPECT_FLOAT_EQ(matrix[3][0], 11); EXPECT_FLOAT_EQ(matrix[3][1], 12); EXPECT_FLOAT_EQ(matrix[3][2],13); EXPECT_FLOAT_EQ(matrix[3][3], 1);

	//EXPECT_EQ(matrix * 2, Matrix4());
}

TEST(Math, MatrixFloatMultiply) {
	Matrix4 matrixA(1, 2, 3, 4,
					5, -1, 6, 7,
					8, 9, 0.5, 10,
					11, 12, 13, -2);

	Matrix4 matrix = matrixA * 2;

	EXPECT_FLOAT_EQ(matrix[0][0], 2);  EXPECT_FLOAT_EQ(matrix[0][1], 4);  EXPECT_FLOAT_EQ(matrix[0][2], 6);  EXPECT_FLOAT_EQ(matrix[0][3], 8);
	EXPECT_FLOAT_EQ(matrix[1][0], 10); EXPECT_FLOAT_EQ(matrix[1][1], -2); EXPECT_FLOAT_EQ(matrix[1][2], 12); EXPECT_FLOAT_EQ(matrix[1][3], 14);
	EXPECT_FLOAT_EQ(matrix[2][0], 16); EXPECT_FLOAT_EQ(matrix[2][1], 18); EXPECT_FLOAT_EQ(matrix[2][2], 1);  EXPECT_FLOAT_EQ(matrix[2][3], 20);
	EXPECT_FLOAT_EQ(matrix[3][0], 22); EXPECT_FLOAT_EQ(matrix[3][1], 24); EXPECT_FLOAT_EQ(matrix[3][2], 26); EXPECT_FLOAT_EQ(matrix[3][3], -4);
}

TEST(Math, MatrixFloatAssignMultiply) {
	Matrix4 matrix(1, 2, 3, 4,
		5, -1, 6, 7,
		8, 9, 0.5, 10,
		11, 12, 13, -2);

	matrix *= 2;

	EXPECT_FLOAT_EQ(matrix[0][0], 2);  EXPECT_FLOAT_EQ(matrix[0][1], 4);  EXPECT_FLOAT_EQ(matrix[0][2], 6);  EXPECT_FLOAT_EQ(matrix[0][3], 8);
	EXPECT_FLOAT_EQ(matrix[1][0], 10); EXPECT_FLOAT_EQ(matrix[1][1], -2); EXPECT_FLOAT_EQ(matrix[1][2], 12); EXPECT_FLOAT_EQ(matrix[1][3], 14);
	EXPECT_FLOAT_EQ(matrix[2][0], 16); EXPECT_FLOAT_EQ(matrix[2][1], 18); EXPECT_FLOAT_EQ(matrix[2][2], 1);  EXPECT_FLOAT_EQ(matrix[2][3], 20);
	EXPECT_FLOAT_EQ(matrix[3][0], 22); EXPECT_FLOAT_EQ(matrix[3][1], 24); EXPECT_FLOAT_EQ(matrix[3][2], 26); EXPECT_FLOAT_EQ(matrix[3][3], -4);
}

TEST(Math, MatrixMatrixMultiply) {
	Matrix4 matrixA(1, 2, 3, 4,
		5, 1, 6, 7,
		8, 9, 1, 10,
		11, 12, 13, 1);

	Matrix4 matrixB(6, 5, 19, -2,
		5, 2, 0, 4,
		-8, 9, 0.5, 10,
		11, -4, 0.13, 1);

	Matrix4 matrix = matrixA * matrixB;

	EXPECT_FLOAT_EQ(matrix[0][0], 36); EXPECT_FLOAT_EQ(matrix[0][1], 20); EXPECT_FLOAT_EQ(matrix[0][2], 21.02); EXPECT_FLOAT_EQ(matrix[0][3], 40);
	EXPECT_FLOAT_EQ(matrix[1][0], 64); EXPECT_FLOAT_EQ(matrix[1][1], 53); EXPECT_FLOAT_EQ(matrix[1][2], 98.91); EXPECT_FLOAT_EQ(matrix[1][3], 61);
	EXPECT_FLOAT_EQ(matrix[2][0], 195); EXPECT_FLOAT_EQ(matrix[2][1], 27); EXPECT_FLOAT_EQ(matrix[2][2], 153.8); EXPECT_FLOAT_EQ(matrix[2][3], 40);
	EXPECT_FLOAT_EQ(matrix[3][0], 33); EXPECT_FLOAT_EQ(matrix[3][1], 192); EXPECT_FLOAT_EQ(matrix[3][2], 215.63); EXPECT_FLOAT_EQ(matrix[3][3], 157);
}

TEST(Math, MatrixMatrixAssignMultiply) {
	Matrix4 matrix(1, 2, 3, 4,
		5, 1, 6, 7,
		8, 9, 1, 10,
		11, 12, 13, 1);

	Matrix4 matrixA(6, 5, 19, -2,
		5, 2, 0, 4,
		-8, 9, 0.5, 10,
		11, -4, 0.13, 1);

	matrix *= matrixA;

	EXPECT_FLOAT_EQ(matrix[0][0], 36); EXPECT_FLOAT_EQ(matrix[0][1], 20); EXPECT_FLOAT_EQ(matrix[0][2], 21.02); EXPECT_FLOAT_EQ(matrix[0][3], 40);
	EXPECT_FLOAT_EQ(matrix[1][0], 64); EXPECT_FLOAT_EQ(matrix[1][1], 53); EXPECT_FLOAT_EQ(matrix[1][2], 98.91); EXPECT_FLOAT_EQ(matrix[1][3], 61);
	EXPECT_FLOAT_EQ(matrix[2][0], 195); EXPECT_FLOAT_EQ(matrix[2][1], 27); EXPECT_FLOAT_EQ(matrix[2][2], 153.8); EXPECT_FLOAT_EQ(matrix[2][3], 40);
	EXPECT_FLOAT_EQ(matrix[3][0], 33); EXPECT_FLOAT_EQ(matrix[3][1], 192); EXPECT_FLOAT_EQ(matrix[3][2], 215.63); EXPECT_FLOAT_EQ(matrix[3][3], 157);
}

TEST(Math, MatrixVectorMultiply) {
	Matrix4 matrixA(1, 2, 3, 4,
		5, 1, 6, 7,
		8, 9, 1, 10,
		11, 12, 13, 1);

	Vector4 vectorA(1, 2, 3, 4);

	auto vector = matrixA * vectorA;
	EXPECT_FLOAT_EQ(vector[0], 30); EXPECT_FLOAT_EQ(vector[1], 53); EXPECT_FLOAT_EQ(vector[2], 69); EXPECT_FLOAT_EQ(vector[3], 78);
}

TEST(Math, MatrixInvert) {
	Matrix4 matrixA(1, 2, 3, 4,
					5, 1, 6, 7,
					8, 9, 1, 10,
					11, 12, 13, 1);

	Matrix4 matrix(Math::Inverse(matrixA));

	EXPECT_FLOAT_EQ(matrix[0][0], -212/507.0f);  EXPECT_FLOAT_EQ(matrix[0][1], 55/338.f);  EXPECT_FLOAT_EQ(matrix[0][2], 157/3042.f);  EXPECT_FLOAT_EQ(matrix[0][3], 53/3042.f);
	EXPECT_FLOAT_EQ(matrix[1][0], 103/507.f); EXPECT_FLOAT_EQ(matrix[1][1], -61/338.f); EXPECT_FLOAT_EQ(matrix[1][2], 127/3042.f); EXPECT_FLOAT_EQ(matrix[1][3], 101/3042.f);
	EXPECT_FLOAT_EQ(matrix[2][0], 79/507.f); EXPECT_FLOAT_EQ(matrix[2][1], 9/338.f); EXPECT_FLOAT_EQ(matrix[2][2], -257/3042.f);  EXPECT_FLOAT_EQ(matrix[2][3], 107/3042.f);
	EXPECT_FLOAT_EQ(matrix[3][0], 23/169.f); EXPECT_FLOAT_EQ(matrix[3][1], 5/169.f); EXPECT_FLOAT_EQ(matrix[3][2], 5/169.f); EXPECT_FLOAT_EQ(matrix[3][3], -8/169.f);
}

TEST(Math, QuaternionMultiply) {
	{
		Quaternion quaternionA(1, 0, 0, 0);
		Quaternion quaternionB(1, 0, 0, 0);

		auto quaternion = quaternionA * quaternionB;
		EXPECT_FLOAT_EQ(quaternion[0], 0); EXPECT_FLOAT_EQ(quaternion[1], 0); EXPECT_FLOAT_EQ(quaternion[2], 0); EXPECT_FLOAT_EQ(quaternion[3], -1);
	}

	{
		Quaternion quaternionA(0, 0.7071067f, 0, 0.7071067f);
		Quaternion quaternionB(0, 0.7071067f, 0, 0.7071067f);
	
		auto quaternion = quaternionA * quaternionB;
		EXPECT_FLOAT_EQ(quaternion[0], 0); EXPECT_FLOAT_EQ(quaternion[1], 1); EXPECT_FLOAT_EQ(quaternion[2], 0); EXPECT_FLOAT_EQ(quaternion[3], 0);
	}
}