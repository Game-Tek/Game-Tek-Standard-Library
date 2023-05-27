#pragma once

#include "../Core.hpp"
#include "Vectors.hpp"
#include "../SIMD.hpp"

//Index increases in row order.

namespace GTSL
{
	template<typename T, uint32 N, uint32 M>
	struct alignas(16) Matrix {
		static constexpr uint32 MATRIX_SIZE = N * M;

		Matrix() {
			for (uint32 i = 0, j = 0; i < N; ++i, ++j) {
				array[i][j] = static_cast<T>(1);
			}
		}

		template<typename... ARGS>
		explicit Matrix(ARGS... args) : array{ args... } {}

		T& operator()(const uint32 x, const uint32 y) { return array[x][y]; }
		T operator()(const uint32 x, const uint32 y) const { return array[x][y]; }

		T* operator[](const uint32 row) { return array[row]; }
		const T* operator[](const uint32 row) const { return array[row]; }

		T array[N][M]{ static_cast<T>(0) };
	};

	/**
	 * \brief Defines a 4x4 matrix with floating point precision in row major order. Is an identity matrix by default.\n
	 * E.J:\n
	 *
	 * Matrix:\n
	 * A B C DestructionTester\n
	 * E F G H\n
	 * I J K L\n
	 * M N O P\n
	 *
	 * Array(data): A, B, C, DestructionTester, E, F, G, H, I, J, K, L, M, N, O, P\n
	 *
	 * Most operations are accelerated by SIMD code.
	 *
	 * Operations follow ordering convention.
	 *
	 */
	struct Matrix4 : Matrix<float, 4, 4> {
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
		explicit Matrix4(const float a) : Matrix{ a, 0.f, 0.f, 0.f, 0.f, a, 0.f, 0.f, 0.f, 0.f, a, 0.f, 0.f, 0.f, 0.f, a }
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
			Matrix{
					row0Column0, row0Column1, row0Column2, row0Column3,
					row1Column0, row1Column1, row1Column2, row1Column3,
					row2Column0, row2Column1, row2Column2, row2Column3,
					row3Column0, row3Column1, row3Column2, row3Column3 }
		{
		}

		~Matrix4() = default;

		explicit Matrix4(const class Quaternion& quaternion);

		explicit Matrix4(const class AxisAngle& axisAngle);

		explicit Matrix4(const Vector3& position);

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
		[[nodiscard]] const float* GetData() const { return array[0]; }

		[[nodiscard]] Vector4 GetXBasisVector() const { return { array[0][0], array[0][1], array[0][2], array[0][3] }; }
		[[nodiscard]] Vector4 GetYBasisVector() const { return { array[1][0], array[1][1], array[1][2], array[1][3] }; }
		[[nodiscard]] Vector4 GetZBasisVector() const { return { array[2][0], array[2][1], array[2][2], array[2][3] }; }
		[[nodiscard]] Vector4 GetWBasisVector() const { return { array[3][0], array[3][1], array[3][2], array[3][3] }; }

		void Transpose() {
			auto a{ float4x(AlignedPointer<const float, 16>(array[0])) }, b{ float4x(AlignedPointer<const float, 16>(array[1])) }, c{ float4x(AlignedPointer<const float, 16>(array[2])) }, d{ float4x(AlignedPointer<const float, 16>(array[3])) };
			float4x::Transpose(a, b, c, d);
			a.CopyTo(UnalignedPointer(array[0])); b.CopyTo(UnalignedPointer(array[1])); c.CopyTo(UnalignedPointer(array[2])); d.CopyTo(UnalignedPointer(array[3]));
		}

		Matrix4& operator+=(const float other) {
			array[0][0] -= other; array[0][1] -= other; array[0][2] -= other; array[0][3] -= other;
			array[1][0] -= other; array[1][1] -= other; array[1][2] -= other; array[1][3] -= other;
			array[2][0] -= other; array[2][1] -= other; array[2][2] -= other; array[2][3] -= other;
			array[3][0] -= other; array[3][1] -= other; array[3][2] -= other; array[3][3] -= other;

			return *this;
		}

		Matrix4& operator+=(const Matrix4& other) {
			array[0][0] += other[0][0]; array[0][1] += other[0][1]; array[0][2] += other[0][2]; array[0][3] += other[0][3];
			array[1][0] += other[1][0]; array[1][1] += other[1][1]; array[1][2] += other[1][2]; array[1][3] += other[1][3];
			array[2][0] += other[2][0]; array[2][1] += other[2][1]; array[2][2] += other[2][2]; array[2][3] += other[2][3];
			array[3][0] += other[3][0]; array[3][1] += other[3][1]; array[3][2] += other[3][2]; array[3][3] += other[3][3];

			return *this;
		}

		Matrix4& operator-=(const float other) {
			array[0][0] -= other; array[0][1] -= other; array[0][2] -= other; array[0][3] -= other;
			array[1][0] -= other; array[1][1] -= other; array[1][2] -= other; array[1][3] -= other;
			array[2][0] -= other; array[2][1] -= other; array[2][2] -= other; array[2][3] -= other;
			array[3][0] -= other; array[3][1] -= other; array[3][2] -= other; array[3][3] -= other;

			return *this;
		}

		Matrix4& operator-=(const Matrix4& other) {
			array[0][0] -= other[0][0]; array[0][1] -= other[0][1]; array[0][2] -= other[0][2]; array[0][3] -= other[0][3];
			array[1][0] -= other[1][0]; array[1][1] -= other[1][1]; array[1][2] -= other[1][2]; array[1][3] -= other[1][3];
			array[2][0] -= other[2][0]; array[2][1] -= other[2][1]; array[2][2] -= other[2][2]; array[2][3] -= other[2][3];
			array[3][0] -= other[3][0]; array[3][1] -= other[3][1]; array[3][2] -= other[3][2]; array[3][3] -= other[3][3];

			return *this;
		}

		Matrix4 operator*(const float other) const {
			Matrix4 result(*this);

			result[0][0] *= other; result[0][1] *= other; result[0][2] *= other; result[0][3] *= other;
			result[1][0] *= other; result[1][1] *= other; result[1][2] *= other; result[1][3] *= other;
			result[2][0] *= other; result[2][1] *= other; result[2][2] *= other; result[2][3] *= other;
			result[3][0] *= other; result[3][1] *= other; result[3][2] *= other; result[3][3] *= other;

			return result;
		}

		Vector3 operator*(const Vector3& other) const {
			Vector3 result;

			result.X() = array[0][0] * other.X() + array[0][1] * other.Y() + array[0][2] * other.Z() + array[0][3] * 1/*W*/;
			result.Y() = array[1][0] * other.X() + array[1][1] * other.Y() + array[1][2] * other.Z() + array[1][3] * 1/*W*/;
			result.Z() = array[2][0] * other.X() + array[2][1] * other.Y() + array[2][2] * other.Z() + array[2][3] * 1/*W*/;

			return result;
		}

		Vector4 operator*(const Vector4& other) const {
			Vector4 result;

			const auto xxxx = float4x(other.X()); const auto yyyy = float4x(other.Y());
			const auto zzzz = float4x(other.Z()); const auto wwww = float4x(other.W());

			const auto p0 = xxxx * float4x(array[0][0], array[1][0], array[2][0], array[3][0]);
			const auto p1 = yyyy * float4x(array[0][1], array[1][1], array[2][1], array[3][1]);
			const auto p2 = zzzz * float4x(array[0][2], array[1][2], array[2][2], array[3][2]);
			const auto p3 = wwww * float4x(array[0][3], array[1][3], array[2][3], array[3][3]);

			auto res = p0 + p1 + p2 + p3; res.CopyTo(AlignedPointer<float, 16>(result.GetData()));

			//result.X() = other.X() * array[0]  + other.Y() * array[1]  + other.Z() * array[2]  + other.W() * array[3];
			//result.Y() = other.X() * array[4]  + other.Y() * array[5]  + other.Z() * array[6]  + other.W() * array[7];
			//result.Z() = other.X() * array[8]  + other.Y() * array[9]  + other.Z() * array[10] + other.W() * array[11];
			//result.W() = other.X() * array[12] + other.Y() * array[13] + other.Z() * array[14] + other.W() * array[15];

			return result;
		}

		Matrix4 operator*(const Matrix4& other) const {
			Matrix4 Result;
			auto Row1 = float4x(AlignedPointer<const float, 16>(other.array[0]));
			auto Row2 = float4x(AlignedPointer<const float, 16>(other.array[1]));
			auto Row3 = float4x(AlignedPointer<const float, 16>(other.array[2]));
			auto Row4 = float4x(AlignedPointer<const float, 16>(other.array[3]));
			float4x Bro1, Bro2, Bro3, Bro4, Row;
			for (uint8 i = 0; i < 4; ++i) {
				Bro1 = array[i][0]; Bro2 = array[i][1]; Bro3 = array[i][2]; Bro4 = array[i][3];
				Row = Bro1 * Row1 + Bro2 * Row2 + Bro3 * Row3 + Bro4 * Row4;
				Row.CopyTo(AlignedPointer<float, 16>(Result.array[i]));
			}
			return Result;
		}

		Matrix4& operator*=(float other) {
			const auto brod = float4x(other);

			for (auto& i : array) {
				auto row = float4x(AlignedPointer<const float, 16>(i));
				row *= brod;
				row.CopyTo(AlignedPointer<float, 16>(i));
			}

			return *this;
		}

		Matrix4& operator*=(const Matrix4& other) {
			//https://codereview.stackexchange.com/questions/101144/simd-matrix-multiplication

			const auto other_row_1 = float4x(AlignedPointer<const float, 16>(other.array[0]));
			const auto other_row_2 = float4x(AlignedPointer<const float, 16>(other.array[1]));
			const auto other_row_3 = float4x(AlignedPointer<const float, 16>(other.array[2]));
			const auto other_row_4 = float4x(AlignedPointer<const float, 16>(other.array[3]));

			float4x this_x, this_y, this_z, this_w, row;

			for (auto& i : array) {
				this_x = i[0]; this_y = i[1]; this_z = i[2]; this_w = i[3];

				row = this_x * other_row_1 + this_y * other_row_2 + this_z * other_row_3 + this_w * other_row_4;

				row.CopyTo(AlignedPointer<float, 16>(i));
			}

			return *this;
		}

		bool operator==(const Matrix4& other) const {
			auto a = array[0][0] == other[0][0] && array[0][1] == other[0][1] && array[0][2] == other[0][2] && array[0][3] == other[0][3];
			auto b = array[1][0] == other[1][0] && array[1][1] == other[1][1] && array[1][2] == other[1][2] && array[1][3] == other[1][3];
			auto c = array[2][0] == other[2][0] && array[2][1] == other[2][1] && array[2][2] == other[2][2] && array[2][3] == other[2][3];
			auto d = array[3][0] == other[3][0] && array[3][1] == other[3][1] && array[3][2] == other[3][2] && array[3][3] == other[3][3];
			return a && b && c && d;
		}
	};

	struct Matrix3x4 : Matrix<float, 3, 4> {
	public:
		Matrix3x4() = default;

		explicit Matrix3x4(const float i) : Matrix{ i, 0.0f, 0.0f, 0.0f, 0.0f, i, 0.0f, 0.0f, 0.0f, 0.0f, i, 0.0f } {}
		explicit Matrix3x4(const Matrix4& matrix4) : Matrix{ matrix4[0][0], matrix4[0][1], matrix4[0][2], matrix4[0][3],
			matrix4[1][0], matrix4[1][1], matrix4[1][2], matrix4[1][3],
			matrix4[2][0], matrix4[2][1], matrix4[2][2], matrix4[2][3] }
		{}
		explicit Matrix3x4(const Vector3 vector3) : Matrix{ 0.f, 0.f, 0.f, vector3.X(), 0.f, 0.f, 0.f, vector3.Y(), 0.f, 0.f, 0.f, vector3.Z() } {}
		explicit Matrix3x4(const class Quaternion& quaternion);

		float* operator[](const uint8 index) { return array[index]; }
		const float* operator[](const uint8 index) const { return array[index]; }

		Vector3 operator*(const Vector3& other) const {
			return { array[0][0] * other.X() + array[0][1] * other.Y() + array[0][2] * other.Z() + array[0][3] * 1/*W*/, array[1][0] * other.X() + array[1][1] * other.Y() + array[1][2] * other.Z() + array[1][3] * 1/*W*/, array[2][0] * other.X() + array[2][1] * other.Y() + array[2][2] * other.Z() + array[2][3] * 1/*W*/ };
		}

		Matrix3x4& operator*=(const Matrix3x4& other) {
			//https://codereview.stackexchange.com/questions/101144/simd-matrix-multiplication

			const auto other_row_1 = float4x(AlignedPointer<const float, 16>(other.array[0]));
			const auto other_row_2 = float4x(AlignedPointer<const float, 16>(other.array[1]));
			const auto other_row_3 = float4x(AlignedPointer<const float, 16>(other.array[2]));
			const auto other_row_4 = float4x(1.0f);

			float4x this_x, this_y, this_z, this_w, row;

			for (auto& i : array) {
				this_x = i[0]; this_y = i[1]; this_z = i[2]; this_w = i[3];

				row = this_x * other_row_1 + this_y * other_row_2 + this_z * other_row_3 + this_w * other_row_4;

				row.CopyTo(AlignedPointer<float, 16>(i));
			}

			return *this;
		}
	private:
		friend Matrix4;
	};

	inline Matrix4::Matrix4(const Vector3& position) {
		(*this)(0, 3) = position.X(); (*this)(1, 3) = position.Y(); (*this)(2, 3) = position.Z();
	}

	inline Matrix4::Matrix4(const Matrix3x4& matrix) : Matrix{
		matrix.array[0][0], matrix.array[0][1], matrix.array[0][2], matrix.array[0][3],
		matrix.array[1][0], matrix.array[1][1], matrix.array[1][2], matrix.array[1][3],
		matrix.array[2][0], matrix.array[2][1], matrix.array[2][2], matrix.array[2][3],
		0.f, 0.f, 0.f, 1.f }
	{

	}
}