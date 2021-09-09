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
	EXPECT_FLOAT_EQ(-1.0f, Math::cos(-GTSL::Math::PI / 2 * 10), 0.00001f);

	EXPECT_FLOAT_EQ(0.7071067f, Math::cos(-GTSL::Math::PI / 4));
	EXPECT_NEAR(0.0f, Math::cos(-GTSL::Math::PI / 2), 0.00001f);

	EXPECT_NEAR(-1.0f, Math::cos(GTSL::Math::PI), 0.00001f);

	EXPECT_NEAR(0.0f, Math::cos(GTSL::Math::PI / 2), 0.00001f);
	EXPECT_FLOAT_EQ(0.7071067f, Math::cos(GTSL::Math::PI / 4));

	EXPECT_FLOAT_EQ(-1.0f, Math::cos(GTSL::Math::PI / 2 * 10));
	EXPECT_NEAR(0.0f, Math::cos(GTSL::Math::PI / 4 * 10), 0.00001f);
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

// Function to get cofactor of A[p][q] in temp[][]. n is current
// dimension of A[][]
void getCofactor(Matrix4 A, Matrix4& temp, int p, int q, int n)
{
	int i = 0, j = 0;

	// Looping for each element of the matrix
	for (int row = 0; row < n; row++)
	{
		for (int col = 0; col < n; col++)
		{
			//  Copying into temporary matrix only those element
			//  which are not in given row and column
			if (row != p && col != q)
			{
				temp[i][j++] = A[row][col];

				// Row is filled, so increase row index and
				// reset col index
				if (j == n - 1)
				{
					j = 0;
					i++;
				}
			}
		}
	}
}

/* Recursive function for finding determinant of matrix.
   n is current dimension of A[][]. */
float32 determinant(Matrix4 A, int n)
{
	float32 D = 0; // Initialize result

	//  Base case : if matrix contains single element
	if (n == 1)
		return A[0][0];

	Matrix4 temp; // To store cofactors

	float32 sign = 1;  // To store sign multiplier

	 // Iterate for each element of first row
	for (int f = 0; f < n; f++)
	{
		// Getting Cofactor of A[0][f]
		getCofactor(A, temp, 0, f, n);
		D += sign * A[0][f] * determinant(temp, n - 1);

		// terms are to be added with alternate sign
		sign = -sign;
	}

	return D;
}

// Function to get adjoint of A[N][N] in adj[N][N].
void adjoint(Matrix4 A, Matrix4& adj)
{
	if (4 == 1)
	{
		adj[0][0] = 1;
		return;
	}

	// temp is used to store cofactors of A[][]
	float32 sign = 1;
	Matrix4 temp;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			// Get cofactor of A[i][j]
			getCofactor(A, temp, i, j, 4);

			// sign of adj[j][i] positive if sum of row
			// and column indexes is even.
			sign = ((i + j) % 2 == 0) ? 1 : -1;

			// Interchanging rows and columns to get the
			// transpose of the cofactor matrix
			adj[j][i] = (sign) * (determinant(temp, 4 - 1));
		}
	}
}

// Function to calculate and store inverse, returns false if
// matrix is singular
bool inverse(Matrix4 A, Matrix4& inverse)
{
	// Find determinant of A[][]
	int det = determinant(A, 4);
	if (det == 0) {
		return false;
	}

	// Find adjoint
	Matrix4 adj;
	adjoint(A, adj);

	// Find Inverse using formula "inverse(A) = adj(A)/det(A)"
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			inverse[i][j] = adj[i][j] / float(det);

	return true;
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