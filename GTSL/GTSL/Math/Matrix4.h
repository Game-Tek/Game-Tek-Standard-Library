#pragma once

#include "GTSL/Core.h"

#include "Vector4.h"
#include "Vector3.h"

//Index increases in row order.

namespace GTSL
{
	
	/**
	 * \brief Defines a 4x4 matrix with floating point precision in row major order. Is an identity matrix by default.\n
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
	 * Operations follow ordering convention.	
	 *
	 */
	class alignas(16) Matrix4
	{
		static constexpr uint8 MATRIX_SIZE = 16;
		
	public:
		/**
		 * \brief Default constructor. Constructs and identity matrix.
		 */
		Matrix4() = default;

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
		explicit Matrix4(const class AxisAngle& axisAngle);
		explicit Matrix4(const Vector3 position);

		explicit Matrix4(const class Matrix3x4& matrix);

		/**
		 * \brief Sets all of this matrices' components to represent an Identity matrix.\n
		 *
		 * 1 0 0 0\n
		 * 0 1 0 0\n
		 * 0 0 1 0\n
		 * 0 0 0 1\n
		 */
		void MakeIdentity() {
			(*this) = Matrix4();
		}

		/**
		 * \brief Returns a pointer to the matrices' data array.
		 * \return const float* to the matrices' data.
		 */
		[[nodiscard]] const float32* GetData() const { return array[0]; }

		[[nodiscard]] Vector4 GetXBasisVector() const { return Vector4(array[0][0], array[0][1], array[0][2], array[0][3]); }
		[[nodiscard]] Vector4 GetYBasisVector() const { return Vector4(array[1][0], array[1][1], array[1][2], array[1][3]); }
		[[nodiscard]] Vector4 GetZBasisVector() const { return Vector4(array[2][0], array[2][1], array[2][2], array[2][3]); }
		[[nodiscard]] Vector4 GetWBasisVector() const { return Vector4(array[3][0], array[3][1], array[3][2], array[3][3]); }
		
		void Transpose();

		Matrix4& operator+=(const float32 other)
		{
			array[0][0] -= other; array[0][1] -= other; array[0][2] -= other; array[0][3] -= other;
			array[1][0] -= other; array[1][1] -= other; array[1][2] -= other; array[1][3] -= other;
			array[2][0] -= other; array[2][1] -= other; array[2][2] -= other; array[2][3] -= other;
			array[3][0] -= other; array[3][1] -= other; array[3][2] -= other; array[3][3] -= other;

			return *this;
		}

		Matrix4& operator+=(const Matrix4& other)
		{
			array[0][0] += other[0][0]; array[0][1] += other[0][1]; array[0][2] += other[0][2]; array[0][3] += other[0][3];
			array[1][0] += other[1][0]; array[1][1] += other[1][1]; array[1][2] += other[1][2]; array[1][3] += other[1][3];
			array[2][0] += other[2][0]; array[2][1] += other[2][1]; array[2][2] += other[2][2]; array[2][3] += other[2][3];
			array[3][0] += other[3][0]; array[3][1] += other[3][1]; array[3][2] += other[3][2]; array[3][3] += other[3][3];

			return *this;
		}

		Matrix4& operator-=(const float other)
		{
			array[0][0] -= other; array[0][1] -= other; array[0][2] -= other; array[0][3] -= other;
			array[1][0] -= other; array[1][1] -= other; array[1][2] -= other; array[1][3] -= other;
			array[2][0] -= other; array[2][1] -= other; array[2][2] -= other; array[2][3] -= other;
			array[3][0] -= other; array[3][1] -= other; array[3][2] -= other; array[3][3] -= other;

			return *this;
		}

		Matrix4& operator-=(const Matrix4& other)
		{
			array[0][0] -= other[0][0]; array[0][1] -= other[0][1]; array[0][2] -= other[0][2]; array[0][3] -= other[0][3];
			array[1][0] -= other[1][0]; array[1][1] -= other[1][1]; array[1][2] -= other[1][2]; array[1][3] -= other[1][3];
			array[2][0] -= other[2][0]; array[2][1] -= other[2][1]; array[2][2] -= other[2][2]; array[2][3] -= other[2][3];
			array[3][0] -= other[3][0]; array[3][1] -= other[3][1]; array[3][2] -= other[3][2]; array[3][3] -= other[3][3];
			
			return *this;
		}

		Matrix4 operator*(const float other) const
		{
			Matrix4 Result;

			Result[0][0] *= other; Result[0][1] *= other; Result[0][2] *= other; Result[0][3] *= other;
			Result[1][0] *= other; Result[1][1] *= other; Result[1][2] *= other; Result[1][3] *= other;
			Result[2][0] *= other; Result[2][1] *= other; Result[2][2] *= other; Result[2][3] *= other;
			Result[3][0] *= other; Result[3][1] *= other; Result[3][2] *= other; Result[3][3] *= other;

			return Result;
		}

		Vector3 operator*(const Vector3& other) const
		{
			Vector3 result;

			result.X() = array[0][0] * other.X() + array[0][1] * other.Y() + array[0][2] * other.Z() + array[0][3] * 1/*W*/;
			result.Y() = array[1][0] * other.X() + array[1][1] * other.Y() + array[1][2] * other.Z() + array[1][3] * 1/*W*/;
			result.Z() = array[2][0] * other.X() + array[2][1] * other.Y() + array[2][2] * other.Z() + array[2][3] * 1/*W*/;

			return result;
		}

		Vector4 operator*(const Vector4& other) const;

		friend Vector4 operator*(const Vector4& vector, const Matrix4& matrix);

		Matrix4 operator*(const Matrix4& other) const;

		Matrix4& operator*=(float other);

		Matrix4& operator*=(const Matrix4& other);

		float32& operator()(const uint8 row, const uint8 column) { return array[row][column]; }
		float32 operator()(const uint8 row, const uint8 column) const { return array[row][column]; }

		float32* operator[](const uint8 row) { return array[row]; }
		const float32* operator[](const uint8 row) const { return array[row]; }
		
	private:
		float32 array[4][4]{ { 1.0f, 0, 0, 0 }, { 0, 1.0f, 0, 0 }, { 0, 0, 1.0f, 0 }, { 0, 0, 0, 1.0f } };
	};

	class Matrix3x4
	{
	public:
		Matrix3x4() = default;

		explicit Matrix3x4(const float32 i) : array{ i, 0.0f, 0.0f, 0.0f, 0.0f, i, 0.0f, 0.0f, 0.0f, 0.0f, i, 0.0f } {}
		explicit Matrix3x4(const Matrix4& matrix4) : array{ matrix4[0][0], matrix4[0][1], matrix4[0][2], matrix4[0][3],
			matrix4[1][0], matrix4[1][1], matrix4[1][2], matrix4[1][3],
			matrix4[2][0], matrix4[2][1], matrix4[2][2], matrix4[2][3] }
		{}
		explicit Matrix3x4(const Vector3 vector3) : array{ 0, 0, 0, vector3.X(), 0, 0, 0, vector3.Y(), 0, 0, 0, vector3.Z() } {}
		explicit Matrix3x4(const class Quaternion& quaternion);
		
		float32* operator[](const uint8 index) { return array[index]; }
		const float32* operator[](const uint8 index) const { return array[index]; }

		Matrix3x4& operator*=(const Matrix3x4& other);
	private:
		float32 array[3][4]{ { 1, 0, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 1, 0 } };

		friend Matrix4;
	};


	inline Matrix4::Matrix4(const Matrix3x4& matrix) : array{
		matrix.array[0][0], matrix.array[0][1], matrix.array[0][2], matrix.array[0][3],
		matrix.array[1][0], matrix.array[1][1], matrix.array[1][2], matrix.array[1][3],
		matrix.array[2][0], matrix.array[2][1], matrix.array[2][2], matrix.array[2][3],
		0, 0, 0, 1 }
	{

	}
}