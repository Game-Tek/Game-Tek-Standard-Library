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
	 * Operation follow ordering convention.	
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

		Matrix4(const Rotator& rotator);
		
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
		 * \param row0Column0 float to set the matrices' Row0; Column0 component as.
		 * \param row0Column1 float to set the matrices' Row0; Column1 component as.
		 * \param row0Column2 float to set the matrices' Row0; Column2 component as.
		 * \param row0Column3 float to set the matrices' Row0; Column3 component as.
		 * \param row1Column0 float to set the matrices' Row1; Column0 component as.
		 * \param row1Column1 float to set the matrices' Row1; Column1 component as.
		 * \param row1Column2 float to set the matrices' Row1; Column2 component as.
		 * \param row1Column3 float to set the matrices' Row1; Column3 component as.
		 * \param row2Column0 float to set the matrices' Row2; Column0 component as.
		 * \param row2Column1 float to set the matrices' Row2; Column1 component as.
		 * \param row2Column2 float to set the matrices' Row2; Column2 component as.
		 * \param row2Column3 float to set the matrices' Row2; Column3 component as.
		 * \param row3Column0 float to set the matrices' Row3; Column0 component as.
		 * \param row3Column1 float to set the matrices' Row3; Column1 component as.
		 * \param row3Column2 float to set the matrices' Row3; Column2 component as.
		 * \param row3Column3 float to set the matrices' Row3; Column3 component as.
		 */
		Matrix4(const float row0Column0, const float row0Column1, const float row0Column2, const float row0Column3,
				const float row1Column0, const float row1Column1, const float row1Column2, const float row1Column3,
				const float row2Column0, const float row2Column1, const float row2Column2, const float row2Column3,
				const float row3Column0, const float row3Column1, const float row3Column2, const float row3Column3) :
		array{
				row0Column0, row0Column1, row0Column2, row0Column3,
				row1Column0, row1Column1, row1Column2, row1Column3,
				row2Column0, row2Column1, row2Column2, row2Column3,
				row3Column0, row3Column1, row3Column2, row3Column3 }
		{
		}

		~Matrix4() = default;

		explicit Matrix4(const class Quaternion& quaternion);
		Matrix4(const class AxisAngle& axisAngle);

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
		
		[[nodiscard]] const float* GetData(uint8 row, uint8 column) const { return &array[row * 4 + column]; }
		[[nodiscard]] float* GetData(uint8 row, uint8 column) { return &array[row * 4 + column]; }

		[[nodiscard]] Vector4 GetXBasisVector() const { return Vector4(array[0], array[1], array[2], array[3]); }
		[[nodiscard]] Vector4 GetYBasisVector() const { return Vector4(array[4], array[5], array[6], array[7]); }
		[[nodiscard]] Vector4 GetZBasisVector() const { return Vector4(array[8], array[9], array[10], array[11]); }
		[[nodiscard]] Vector4 GetWBasisVector() const { return Vector4(array[12], array[13], array[14], array[15]); }
		
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

		Matrix4 operator*(const Matrix4& other) const;

		Matrix4& operator*=(float other);

		Matrix4& operator*=(const Matrix4& other);

		float& operator[](const uint8 index) { return array[index]; }
		float operator[](const uint8 index) const { return array[index]; }

		float& operator()(const uint8 row, const uint8 column) { return array[row * 4 + column]; }
		float operator()(const uint8 row, const uint8 column) const { return array[row * 4 + column]; }

	private:
		float32 array[MATRIX_SIZE];
	};

	class Matrix3x4
	{
	public:
		Matrix3x4() = default;

		explicit Matrix3x4(const float32 i) : array{ i, 0.0f, 0.0f, 0.0f, 0.0f, i, 0.0f, 0.0f, 0.0f, 0.0f, i, 0.0f } {}
		explicit Matrix3x4(const Matrix4& matrix4) : array{ matrix4[0], matrix4[1], matrix4[2], matrix4[3], matrix4[4], matrix4[5], matrix4[6], matrix4[7], matrix4[8], matrix4[9], matrix4[10], matrix4[11] } {}
		
		float32& operator[](const uint8 index) { return array[index]; }
		float32 operator[](const uint8 index) const { return array[index]; }
	private:
		float32 array[12]{ 0.0f };
	};
}