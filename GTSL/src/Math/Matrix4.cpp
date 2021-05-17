#include "GTSL/Math/Matrix4.h"


#include "GTSL/Math/AxisAngle.h"
#include "GTSL/Math/Quaternion.h"
#include "GTSL/Math/Math.hpp"
#include "GTSL/SIMD/SIMD128.hpp"

using namespace GTSL;

//CODE IS CORRECT
Matrix4::Matrix4(const Quaternion& quaternion)
{
	const auto xx = quaternion.X() * quaternion.X();
	const auto xy = quaternion.X() * quaternion.Y();
	const auto xz = quaternion.X() * quaternion.Z();
	const auto xw = quaternion.X() * quaternion.W();
	const auto yy = quaternion.Y() * quaternion.Y();
	const auto yz = quaternion.Y() * quaternion.Z();
	const auto yw = quaternion.Y() * quaternion.W();
	const auto zz = quaternion.Z() * quaternion.Z();
	const auto zw = quaternion.Z() * quaternion.W();
	
	array[0][0] = 1 - 2 * (yy + zz);
	array[0][1] = 2 * (xy - zw);
	array[0][2] = 2 * (xz + yw);
	array[1][0] = 2 * (xy + zw);
	array[1][1] = 1 - 2 * (xx + zz);
	array[1][2] = 2 * (yz - xw);
	array[2][0] = 2 * (xz - yw);
	array[2][1] = 2 * (yz + xw);
	array[2][2] = 1 - 2 * (xx + yy);
	array[0][3] = array[1][3] = array[2][3] = array[3][0] = array[3][1] = array[3][2] = 0;
	array[3][3] = 1;
}

Matrix4::Matrix4(const AxisAngle& axisAngle) : Matrix4(1)
{
	const float32 c = Math::Cosine(axisAngle.Angle);
	const float32 s = Math::Sine(axisAngle.Angle);
	const float32 t = 1.0f - c;
	const float32 xx = axisAngle.X * axisAngle.X;
	const float32 xy = axisAngle.X * axisAngle.Y;
	const float32 xz = axisAngle.X * axisAngle.Z;
	const float32 yy = axisAngle.Y * axisAngle.Y;
	const float32 yz = axisAngle.Y * axisAngle.Z;
	const float32 zz = axisAngle.Z * axisAngle.Z;

	// build rotation matrix
	array[0][0] = c + xx * t;
	array[1][1] = c + yy * t;
	array[2][2] = c + zz * t;

	auto tmp1 = xy * t;
	auto tmp2 = axisAngle.Z * s;

	array[1][0] = tmp1 + tmp2;
	array[0][1] = tmp1 - tmp2;

	tmp1 = xz * t;
	tmp2 = -axisAngle.Y * s;

	array[2][0] = tmp1 - tmp2;
	array[0][2] = tmp1 + tmp2;

	tmp1 = yz * t;
	tmp2 = axisAngle.X * s;

	array[2][1] = tmp1 + tmp2;
	array[1][2] = tmp1 - tmp2;
}

Matrix4::Matrix4(const Vector3 position)
{
	(*this)(0, 3) = position.X(); (*this)(1, 3) = position.Y(); (*this)(2, 3) = position.Z();
}


//CODE IS CORRECT
Matrix4::Matrix4(const Rotator& rotator) : Matrix4(1)
{
	//Heading = rotation about y axis
	//Attitude = rotation about z axis
	//Bank = rotation about x axis
	
	float32 ch = Math::Cosine(rotator.Y);
	float32 sh = Math::Sine(rotator.Y);
	float32 ca = Math::Cosine(rotator.Z);
	float32 sa = Math::Sine(rotator.Z);
	float32 cb = Math::Cosine(rotator.X);
	float32 sb = Math::Sine(rotator.X);

	(*this)(0, 0) = ch * ca;
	(*this)(0, 1) = sh * sb - ch * sa * cb;
	(*this)(0, 2) = ch * sa * sb + sh * cb;
	(*this)(1, 0) = sa;
	(*this)(1, 1) = ca * cb;
	(*this)(1, 2) = -ca * sb;
	(*this)(2, 0) = -sh * ca;
	(*this)(2, 1) = sh * sa * cb + ch * sb;
	(*this)(2, 2) = -sh * sa * sb + ch * cb;
}

void Matrix4::Transpose()
{
	auto a{ SIMD128<float32>(AlignedPointer<const float32, 16>(array[0])) }, b{ SIMD128<float32>(AlignedPointer<const float32, 16>(array[1])) }, c{ SIMD128<float32>(AlignedPointer<const float32, 16>(array[2])) }, d{ SIMD128<float32>(AlignedPointer<const float32, 16>(array[3])) };
	SIMD128<float32>::Transpose(a, b, c, d);
	a.CopyTo(UnalignedPointer<float32>(array[0])); b.CopyTo(UnalignedPointer<float32>(array[1])); c.CopyTo(UnalignedPointer<float32>(array[2])); d.CopyTo(UnalignedPointer<float32>(array[3]));
}

Vector4 Matrix4::operator*(const Vector4& other) const
{
	Vector4 result;

	const auto xxxx = SIMD128<float32>(other.X()); const auto yyyy = SIMD128<float32>(other.Y());
	const auto zzzz = SIMD128<float32>(other.Z()); const auto wwww = SIMD128<float32>(other.W());

	const auto p0 = xxxx * SIMD128<float32>(array[0][0], array[1][0], array[2][0], array[3][0]);
	const auto p1 = yyyy * SIMD128<float32>(array[0][1], array[1][1], array[2][1], array[3][1]);
	const auto p2 = zzzz * SIMD128<float32>(array[0][2], array[1][2], array[2][2], array[3][2]);
	const auto p3 = wwww * SIMD128<float32>(array[0][3], array[1][3], array[2][3], array[3][3]);

	auto res = p0 + p1 + p2 + p3; res.CopyTo(AlignedPointer<float32, 16>(result.GetData()));

	//result.X() = other.X() * array[0]  + other.Y() * array[1]  + other.Z() * array[2]  + other.W() * array[3];
	//result.Y() = other.X() * array[4]  + other.Y() * array[5]  + other.Z() * array[6]  + other.W() * array[7];
	//result.Z() = other.X() * array[8]  + other.Y() * array[9]  + other.Z() * array[10] + other.W() * array[11];
	//result.W() = other.X() * array[12] + other.Y() * array[13] + other.Z() * array[14] + other.W() * array[15];
	
	return result;
}

//WORKS
Matrix4 Matrix4::operator*(const Matrix4& other) const
{
	Matrix4 Result;

	auto Row1 = SIMD128<float32>(AlignedPointer<const float32, 16>(other.array[0]));
	auto Row2 = SIMD128<float32>(AlignedPointer<const float32, 16>(other.array[1]));
	auto Row3 = SIMD128<float32>(AlignedPointer<const float32, 16>(other.array[2]));
	auto Row4 = SIMD128<float32>(AlignedPointer<const float32, 16>(other.array[3]));

	SIMD128<float32> Bro1, Bro2, Bro3, Bro4, Row;

	for (uint8 i = 0; i < 4; ++i)
	{
		Bro1 = array[i][0]; Bro2 = array[i][1]; Bro3 = array[i][2]; Bro4 = array[i][3];

		Row = Bro1 * Row1 + Bro2 * Row2 + Bro3 * Row3 + Bro4 * Row4;

		Row.CopyTo(AlignedPointer<float32, 16>(Result.array[i]));
	}

	return Result;
}

Matrix4& Matrix4::operator*=(const float32 other)
{
	const auto brod = SIMD128<float32>(other);
	
	for(uint8 i = 0; i < 4; ++i)
	{
		auto row = SIMD128<float32>(AlignedPointer<const float32, 16>(array[i]));
		row *= brod;
		row.CopyTo(AlignedPointer<float32, 16>(array[i]));
	}

	return *this;
}

//WORKS
Matrix4& Matrix4::operator*=(const Matrix4& other)
{
	//https://codereview.stackexchange.com/questions/101144/simd-matrix-multiplication

	const auto other_row_1 = SIMD128<float32>(AlignedPointer<const float32, 16>(other.array[0]));
	const auto other_row_2 = SIMD128<float32>(AlignedPointer<const float32, 16>(other.array[1]));
	const auto other_row_3 = SIMD128<float32>(AlignedPointer<const float32, 16>(other.array[2]));
	const auto other_row_4 = SIMD128<float32>(AlignedPointer<const float32, 16>(other.array[3]));

	SIMD128<float32> this_x, this_y, this_z, this_w, row;

	for (uint8 i = 0; i < 4; ++i)
	{
		this_x = array[i][0]; this_y = array[i][1]; this_z = array[i][2]; this_w = array[i][3];

		row = this_x * other_row_1 + this_y * other_row_2 + this_z * other_row_3 + this_w * other_row_4;

		row.CopyTo(AlignedPointer<float32, 16>(array[i]));
	}

	return *this;
}

Matrix3x4::Matrix3x4(const Quaternion& quaternion)
{
	const auto xx = quaternion.X() * quaternion.X();
	const auto xy = quaternion.X() * quaternion.Y();
	const auto xz = quaternion.X() * quaternion.Z();
	const auto xw = quaternion.X() * quaternion.W();
	const auto yy = quaternion.Y() * quaternion.Y();
	const auto yz = quaternion.Y() * quaternion.Z();
	const auto yw = quaternion.Y() * quaternion.W();
	const auto zz = quaternion.Z() * quaternion.Z();
	const auto zw = quaternion.Z() * quaternion.W();

	array[0][0] = 1 - 2 * (yy + zz);
	array[0][1] = 2 * (xy - zw);
	array[0][2] = 2 * (xz + yw);
	array[1][0] = 2 * (xy + zw);
	array[1][1] = 1 - 2 * (xx + zz);
	array[1][2] = 2 * (yz - xw);
	array[2][0] = 2 * (xz - yw);
	array[2][1] = 2 * (yz + xw);
	array[2][2] = 1 - 2 * (xx + yy);
	array[0][3] = array[1][3] = array[2][3] = 0;
}

Matrix3x4& Matrix3x4::operator*=(const Matrix3x4& other)
{
	//https://codereview.stackexchange.com/questions/101144/simd-matrix-multiplication

	const auto other_row_1 = SIMD128<float32>(AlignedPointer<const float32, 16>(other.array[0]));
	const auto other_row_2 = SIMD128<float32>(AlignedPointer<const float32, 16>(other.array[1]));
	const auto other_row_3 = SIMD128<float32>(AlignedPointer<const float32, 16>(other.array[2]));
	const auto other_row_4 = SIMD128<float32>(1.0f);

	SIMD128<float32> this_x, this_y, this_z, this_w, row;

	for (uint8 i = 0; i < 3; ++i)
	{
		this_x = array[i][0]; this_y = array[i][1]; this_z = array[i][2]; this_w = array[i][3];

		row = this_x * other_row_1 + this_y * other_row_2 + this_z * other_row_3 + this_w * other_row_4;

		row.CopyTo(AlignedPointer<float32, 16>(array[i]));
	}

	return *this;
}

Vector4 GTSL::operator*(const Vector4& vector, const Matrix4& matrix)
{
	GTSL::Vector4 result;
	
	result.X() = vector.X() * matrix.array[0][0] + vector.Y() * matrix.array[1][0] + vector.Z() * matrix.array[2][0] + vector.W() * matrix.array[3][0];
	result.Y() = vector.X() * matrix.array[0][1] + vector.Y() * matrix.array[1][1] + vector.Z() * matrix.array[2][1] + vector.W() * matrix.array[3][1];
	result.Z() = vector.X() * matrix.array[0][2] + vector.Y() * matrix.array[1][2] + vector.Z() * matrix.array[2][2] + vector.W() * matrix.array[3][2];
	result.W() = vector.X() * matrix.array[0][3] + vector.Y() * matrix.array[1][3] + vector.Z() * matrix.array[2][3] + vector.W() * matrix.array[3][3];

	return result;
}
