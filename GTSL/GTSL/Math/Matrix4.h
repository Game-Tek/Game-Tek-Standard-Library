#pragma once

#include "GTSL/Core.h"

#include "Vector4.h"
#include "Vector3.h"

//Index increases in row order.

namespace GTSL
{
	
	/**
	 * \brief Defines a 4x4 matrix with floating point precision.\n
	 * vector is stored in row major order.
	 * E.J:\n
	 *
	 * Matrix:\n
	 * A B C D\n
	 * E F G H\n
	 * I J K L\n
	 * M N O P\n
	 *
	 * Array(data): A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P\n
	 *
	 * Most operations are accelerated by SIMD code.
	 *
	 */
	class alignas(16) Matrix4
	{
		static constexpr uint8 MATRIX_SIZE = 16;
		
	public:
		/**
		 * \brief Default constructor. Sets all of the matrices' components as 0.
		 */
		Matrix4() : array{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
		{
		}

		/**
		 * \brief Builds an identity matrix with _A being each of the identity elements value.
		 * Usually one(1) will be used.
		 * \n
		 * \n
		 * _A 0 0 0\n
		 * 0 _A 0 0\n
		 * 0 0 _A 0\n
		 * 0 0 0 _A\n
		 *
		 * \param a float to set each of the matrix identity elements value as.
		 */
		explicit Matrix4(const float32 a) : array{ a, 0, 0, 0, 0, a, 0, 0, 0, 0, a, 0, 0, 0, 0, a }
		{
		}

		/**
		 * \brief Constructs the matrix with every component set as the corresponding parameter.
		 * \param row0_Column0 float to set the matrices' Row0; Column0 component as.
		 * \param row0_Column1 float to set the matrices' Row0; Column1	component as.
		 * \param row0_Column2 float to set the matrices' Row0; Column2	component as.
		 * \param row0_Column3 float to set the matrices' Row0; Column3	component as.
		 * \param row1_Column0 float to set the matrices' Row1; Column0	component as.
		 * \param row1_Column1 float to set the matrices' Row1; Column1	component as.
		 * \param row1_Column2 float to set the matrices' Row1; Column2	component as.
		 * \param row1_Column3 float to set the matrices' Row1; Column3	component as.
		 * \param row2_Column0 float to set the matrices' Row2; Column0	component as.
		 * \param row2_Column1 float to set the matrices' Row2; Column1	component as.
		 * \param row2_Column2 float to set the matrices' Row2; Column2	component as.
		 * \param row2_Column3 float to set the matrices' Row2; Column3	component as.
		 * \param row3_Column0 float to set the matrices' Row3; Column0	component as.
		 * \param row3_Column1 float to set the matrices' Row3; Column1	component as.
		 * \param row3_Column2 float to set the matrices' Row3; Column2	component as.
		 * \param row3_Column3 float to set the matrices' Row3; Column3	component as.
		 */
		Matrix4(const float row0_Column0, const float row0_Column1, const float row0_Column2, const float row0_Column3,
			const float row1_Column0, const float row1_Column1, const float row1_Column2, const float row1_Column3,
			const float row2_Column0, const float row2_Column1, const float row2_Column2, const float row2_Column3,
			const float row3_Column0, const float row3_Column1, const float row3_Column2,
			const float row3_Column3) :
			array{
				row0_Column0, row0_Column1, row0_Column2, row0_Column3,
				row1_Column0, row1_Column1, row1_Column2, row1_Column3,
				row2_Column0, row2_Column1, row2_Column2, row2_Column3,
				row3_Column0, row3_Column1, row3_Column2, row3_Column3
		}
		{
		}

		~Matrix4() = default;

		explicit Matrix4(const class Quaternion& quaternion);
		explicit Matrix4(const class Rotator& rotator);

		/**
		 * \brief Sets all of this matrices' components to represent an Identity matrix.\n
		 *
		 * 1 0 0 0\n
		 * 0 1 0 0\n
		 * 0 0 1 0\n
		 * 0 0 0 1\n
		 */
		void MakeIdentity()
		{
			for (auto& element : array) { element = 0.0f; }

			array[0] = 1.0f; array[5] = 1.0f; array[10] = 1.0f; array[15] = 1.0f;
		}

		/**
		 * \brief Returns a pointer to the matrices' data array.
		 * \return const float* to the matrices' data.
		 */
		[[nodiscard]] const float* GetData() const { return array; }

		void Transpose();

		Matrix4 operator+(const float other) const
		{
			Matrix4 Result;

			for (uint8 i = 0; i < MATRIX_SIZE; i++)
			{
				Result[i] += other;
			}

			return Result;
		}

		Matrix4 operator+(const Matrix4& other) const
		{
			Matrix4 Result;

			for (uint8 i = 0; i < MATRIX_SIZE; i++)
			{
				Result[i] += other[i];
			}

			return Result;
		}

		Matrix4& operator+=(const float other)
		{
			for (float& i : array)
			{
				i += other;
			}

			return *this;
		}

		Matrix4& operator+=(const Matrix4& other)
		{
			for (uint8 i = 0; i < MATRIX_SIZE; i++)
			{
				array[i] += other[i];
			}

			return *this;
		}

		Matrix4 operator-(const float other) const
		{
			Matrix4 Result;

			for (uint8 i = 0; i < MATRIX_SIZE; i++)
			{
				Result[i] -= other;
			}

			return Result;
		}

		Matrix4 operator-(const Matrix4& other) const
		{
			Matrix4 Result;

			for (uint8 i = 0; i < MATRIX_SIZE; i++)
			{
				Result[i] -= other[i];
			}

			return Result;
		}

		Matrix4& operator-=(const float other)
		{
			for (uint8 i = 0; i < MATRIX_SIZE; i++)
			{
				array[i] -= other;
			}

			return *this;
		}

		Matrix4& operator-=(const Matrix4& other)
		{
			for (uint8 i = 0; i < MATRIX_SIZE; i++)
			{
				array[i] -= other[i];
			}

			return *this;
		}

		Matrix4 operator*(const float other) const
		{
			Matrix4 Result;

			for (uint8 i = 0; i < MATRIX_SIZE; i++)
			{
				Result[i] *= other;
			}

			return Result;
		}

		Vector3 operator*(const Vector3& other) const
		{
			Vector3 Result;

			Result.X = array[0] * other.X + array[1] * other.X + array[2] * other.X + array[3] * other.X;
			Result.Y = array[4] * other.Y + array[5] * other.Y + array[6] * other.Y + array[7] * other.Y;
			Result.Z = array[8] * other.Z + array[9] * other.Z + array[10] * other.Z + array[11] * other.Z;

			return Result;
		}

		Vector4 operator*(const Vector4& other) const;
		//{
		//	Vector4 Result;
		//
		//	Result.X = Array[0] * other.X + Array[1] * other.X + Array[2] * other.X + Array[3] * other.X;
		//	Result.Y = Array[4] * other.Y + Array[5] * other.Y + Array[6] * other.Y+ Array[7] * other.Y;
		//	Result.Z = Array[8] * other.Z + Array[9] * other.Z + Array[10] * other.Z + Array[11] * other.Z;
		//	Result.W = Array[12] * other.W + Array[13] * other.W + Array[14] * other.W + Array[15] * other.W;
		//
		//	return Result;
		//}

		Matrix4 operator*(const Matrix4& other) const;

		//Matrix4 operator* (const Matrix4& other) const
		//{
		//	Matrix4 Result(1);
		//
		//	for (int i = 0; i < 4; i++)
		//	{
		//		for (int j = 0; j < 4; j++)
		//		{
		//			Result[i + j] = 0;
		//			for (int k = 0; k < 4; k++)
		//			{
		//				Result[i + j] += Array[i + k] * other[k + j];
		//			}
		//		}
		//	}
		//
		//	return Result;
		//}

		Matrix4& operator*=(float other);
		//{
		//	for (uint8 i = 0; i < MATRIX_SIZE; i++)
		//	{
		//		Array[i] *= other;
		//	}
		//
		//	return *this;
		//}

		Matrix4& operator*=(const Matrix4& other);
		//{
		//	for (uint8 y = 0; y < 4; y++)
		//	{
		//		for (uint8 x = 0; x < 4; x++)
		//		{
		//			float Sum = 0.0f;
		//
		//			for (uint8 e = 0; e < 4; e++)
		//			{
		//				Sum += Array[e + y * 4] * other[x + e * 4];
		//			}
		//
		//			Array[x + y * 4] = Sum;
		//		}
		//	}
		//
		//	return *this;
		//}

		float& operator[](const uint8 index) { return array[index]; }
		float operator[](const uint8 index) const { return array[index]; }

		float& operator()(const uint8 row, const uint8 column) { return array[row * 4 + column]; }
		float operator()(const uint8 row, const uint8 column) const { return array[row * 4 + column]; }

	private:
		float32 array[MATRIX_SIZE];
	};
}