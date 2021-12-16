#include <gtest/gtest.h>

#include "GTSL/Vector.hpp"
#include "GTSL/Math/Math.hpp"

using namespace GTSL;

TEST(Math, Sines) {
	EXPECT_FLOAT_EQ(0.0f,			Math::Sine(0.0f));

	EXPECT_FLOAT_EQ(-1.0f,			Math::Sine(-GTSL::Math::PI / 4 * 10));
	EXPECT_NEAR(0.0f,				Math::Sine(-GTSL::Math::PI / 2 * 10), 0.00001f);

	EXPECT_FLOAT_EQ(-0.7071067f,	Math::Sine(-GTSL::Math::PI / 4));
	EXPECT_FLOAT_EQ(-1.0f,			Math::Sine(-GTSL::Math::PI / 2));

	EXPECT_NEAR(0.0f,				Math::Sine(GTSL::Math::PI), 0.00001f);

	EXPECT_FLOAT_EQ(1.0f,			Math::Sine(GTSL::Math::PI / 2));
	EXPECT_FLOAT_EQ(0.7071067f,		Math::Sine(GTSL::Math::PI / 4));

	EXPECT_NEAR(0.0f,				Math::Sine(GTSL::Math::PI / 2 * 10), 0.00001f);
	EXPECT_FLOAT_EQ(1.0f,			Math::Sine(GTSL::Math::PI / 4 * 10));

	for(float32 i = -1000.f; i <= 1000.f; ++i) {
		EXPECT_NEAR(sinf(i), Math::Sine(i), 0.00008f);
	}

	alignas(16) float32 source[8] = { 2.0f, 3.f, -1.f, 8.f, 9.f, -5.f, 3.14159f, -8.9f };

	alignas(16) float32 results[8];

	Math::Sine({ 8, source }, { 8, results });

	for(uint32 i = 0; i < 8; ++i) {
		EXPECT_NEAR(Math::Sine(source[i]), results[i], 0.00001f);
	}

}

TEST(Math, Cosines) {
	EXPECT_FLOAT_EQ(1.0f, Math::Cosine(0.0f));

	EXPECT_NEAR(0.0f, Math::Cosine(-GTSL::Math::PI / 4 * 10), 0.00001f);
	EXPECT_FLOAT_EQ(-1.0f, Math::Cosine(-GTSL::Math::PI / 2 * 10));

	EXPECT_FLOAT_EQ(0.7071067f, Math::Cosine(-GTSL::Math::PI / 4));
	EXPECT_NEAR(0.0f, Math::Cosine(-GTSL::Math::PI / 2), 0.00001f);

	EXPECT_NEAR(-1.0f, Math::Cosine(GTSL::Math::PI), 0.00001f);

	EXPECT_NEAR(0.0f, Math::Cosine(GTSL::Math::PI / 2), 0.00001f);
	EXPECT_FLOAT_EQ(0.7071067f, Math::Cosine(GTSL::Math::PI / 4));

	EXPECT_FLOAT_EQ(-1.0f, Math::Cosine(GTSL::Math::PI / 2 * 10));
	EXPECT_NEAR(0.0f, Math::Cosine(GTSL::Math::PI / 4 * 10), 0.00001f);

	for (float32 i = -1000.f; i <= 1000.f; ++i) {
		EXPECT_NEAR(cosf(i), Math::Cosine(i), 0.00009f);
	}

	alignas(16) float32 source[8] = { 2.0f, 3.f, -1.f, 8.f, 9.f, -5.f, 3.14159f, -8.9f };

	alignas(16) float32 results[8];

	Math::Cosine({ 8, source }, { 8, results });

	for (uint32 i = 0; i < 8; ++i) {
		EXPECT_NEAR(Math::Cosine(source[i]), results[i], 0.00001f);
	}
}

TEST(Math, Tangent) {
	EXPECT_NEAR(tanf(0), GTSL::Math::Tangent(0.0f), 0.000005f);
	EXPECT_NEAR(tanf(5), GTSL::Math::Tangent(5.0f), 0.000005f);
	EXPECT_NEAR(tanf(45), GTSL::Math::Tangent(45.0f), 0.00005f);
	EXPECT_NEAR(tanf(90), GTSL::Math::Tangent(90.0f), 0.00008f);
	EXPECT_NEAR(tanf(180), GTSL::Math::Tangent(180.0f), 0.000008f);

	alignas(16) float32 source[8] = { 2.0f, 3.f, -1.f, 8.f, 9.f, -5.f, 3.14159f, -8.9f };

	alignas(16) float32 results[8];

	Math::Tangent({ 8, source }, { 8, results });

	for (uint32 i = 0; i < 8; ++i) {
		EXPECT_NEAR(tanf(source[i]), results[i], 0.000001f);
	}
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

TEST(Math, ProjectionMatrix) {
	auto o = Math::BuildPerspectiveMatrix(45, 16.0f / 9.0f, 1.0f, 100.0f);
	auto i = Math::BuildInvertedPerspectiveMatrix(45, 16.0f / 9.0f, 1.0f, 100.0f);

	auto inverted = Math::Inverse(o);

	EXPECT_NEAR(i[0][0], inverted[0][0], 0.000001f); EXPECT_NEAR(i[0][1], inverted[0][1], 0.000001f); EXPECT_NEAR(i[0][2], inverted[0][2], 0.000001f); EXPECT_NEAR(i[0][3], inverted[0][3], 0.000001f);
	EXPECT_NEAR(i[1][0], inverted[1][0], 0.000001f); EXPECT_NEAR(i[1][1], inverted[1][1], 0.000001f); EXPECT_NEAR(i[1][2], inverted[1][2], 0.000001f); EXPECT_NEAR(i[1][3], inverted[1][3], 0.000001f);
	EXPECT_NEAR(i[2][0], inverted[2][0], 0.000001f); EXPECT_NEAR(i[2][1], inverted[2][1], 0.000001f); EXPECT_NEAR(i[2][2], inverted[2][2], 0.000001f); EXPECT_NEAR(i[2][3], inverted[2][3], 0.000001f);
	EXPECT_NEAR(i[3][0], inverted[3][0], 0.000001f); EXPECT_NEAR(i[3][1], inverted[3][1], 0.000001f); EXPECT_NEAR(i[3][2], inverted[3][2], 0.000001f); EXPECT_NEAR(i[3][3], inverted[3][3], 0.000001f);
}

TEST(Math, DotProduct) {
	alignas(32) float32 xxxx[] = { 1.0f, 5.0f, 4.0f, 8.0f, 2.33f, 2.77f, 8.1f, 9.3f, 4.5f };
	alignas(32) float32 yyyy[] = { 2.5f, 1.1f, 7.6f, 9.2f, 4.22f, 7.2f, 4.1f, 11.11f, 12.3f };
	alignas(32) float32 zzzz[] = { 3.f, 9.f, 2.f, 1.f, 7.f };
	alignas(32) float32 wwww[] = { 7.f, 9.5f, 2.3f, 1.7f, 4.54f };

	{
		float32 alignas(32) res[9];

		GTSL::Math::DotProduct(res, MultiRange<const float, const float>(9u, xxxx, yyyy), GTSL::Vector2(1.0f, 2.0f));

		for (uint32 i = 0; i < 9; ++i) {
			ASSERT_FLOAT_EQ(res[i], xxxx[i] * 1.0f + yyyy[i] * 2.0f);
		}
	}
}