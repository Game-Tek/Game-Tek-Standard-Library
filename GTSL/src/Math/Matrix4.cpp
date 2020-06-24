#include "GTSL/Math/Matrix4.h"

#include "GTSL/Math/Quaternion.h"
#include "GTSL/Math/Math.hpp"
#include "GTSL/SIMD/SIMD128.hpp"

using namespace GTSL;

//CODE IS CORRECT
Matrix4::Matrix4(const Quaternion& quaternion)
{
	const auto xx = quaternion.X * quaternion.X;
	const auto xy = quaternion.X * quaternion.Y;
	const auto xz = quaternion.X * quaternion.Z;
	const auto xw = quaternion.X * quaternion.Q;
	const auto yy = quaternion.Y * quaternion.Y;
	const auto yz = quaternion.Y * quaternion.Z;
	const auto yw = quaternion.Y * quaternion.Q;
	const auto zz = quaternion.Z * quaternion.Z;
	const auto zw = quaternion.Z * quaternion.Q;

	array[0] = 1 - 2 * (yy + zz);
	array[1] = 2 * (xy - zw);
	array[2] = 2 * (xz + yw);
	array[4] = 2 * (xy + zw);
	array[5] = 1 - 2 * (xx + zz);
	array[6] = 2 * (yz - xw);
	array[8] = 2 * (xz - yw);
	array[9] = 2 * (yz + xw);
	array[10] = 1 - 2 * (xx + yy);
	array[3] = array[7] = array[11] = array[12] = array[13] = array[14] = 0;
	array[15] = 1;
}


//CODE IS CORRECT
Matrix4::Matrix4(const Rotator& rotator) : Matrix4(1)
{
	float SP, SY, SR;
	float CP, CY, CR;

	Math::SinCos(&SP, &CP, rotator.X);
	Math::SinCos(&SY, &CY, rotator.Y);
	Math::SinCos(&SR, &CR, rotator.Z);

	array[0] = CP * CY;						array[1] = CP * SY;					array[2] = SP;			array[3] = 0.f;
	array[4] = SR * SP * CY - CR * SY;		array[5] = SR * SP * SY + CR * CY;	array[6] = -SR * CP;	array[7] = 0.f;
	array[8] = -(CR * SP * CY + SR * SY);	array[9] = CY * SR - CR * SP * SY;	array[10] = CR * CP;	array[11] = 0.f;
	array[12] = 0;							array[13] = 0;						array[14] = 0;			array[15] = 1;
}

void Matrix4::Transpose()
{
	auto a{ SIMD128<float32>(AlignedPointer<const float32, 16>(&array[0])) }, b{ SIMD128<float32>(AlignedPointer<const float32, 16>(&array[4])) }, c{ SIMD128<float32>(AlignedPointer<const float32, 16>(&array[8])) }, d{ SIMD128<float32>(AlignedPointer<const float32, 16>(&array[12])) };
	SIMD128<float32>::Transpose(a, b, c, d);
	a.CopyTo(UnalignedPointer<float32>(&array[0])); b.CopyTo(UnalignedPointer<float32>(&array[4])); c.CopyTo(UnalignedPointer<float32>(&array[8])); d.CopyTo(UnalignedPointer<float32>(&array[12]));
}

Vector4 Matrix4::operator*(const Vector4& other) const
{
	alignas(16) Vector4 Result;

	const auto P1(SIMD128<float32>(AlignedPointer<const float32, 16>(&other.X)) * SIMD128<float32>(AlignedPointer<const float32, 16>(&array[0])));
	const auto P2(SIMD128<float32>(AlignedPointer<const float32, 16>(&other.Y)) * SIMD128<float32>(AlignedPointer<const float32, 16>(&array[4])));
	const auto P3(SIMD128<float32>(AlignedPointer<const float32, 16>(&other.Z)) * SIMD128<float32>(AlignedPointer<const float32, 16>(&array[8])));
	const auto P4(SIMD128<float32>(AlignedPointer<const float32, 16>(&other.W)) * SIMD128<float32>(AlignedPointer<const float32, 16>(&array[12])));

	SIMD128<float32>(P1 + P2 + P3 + P4).CopyTo(AlignedPointer<float32, 16>(&Result.X));

	return Result;
}

Matrix4 Matrix4::operator*(const Matrix4& other) const
{
	Matrix4 Result;

	auto Row1 = SIMD128<float32>(AlignedPointer<const float32, 16>(&other.array[0]));
	auto Row2 = SIMD128<float32>(AlignedPointer<const float32, 16>(&other.array[4]));
	auto Row3 = SIMD128<float32>(AlignedPointer<const float32, 16>(&other.array[8]));
	auto Row4 = SIMD128<float32>(AlignedPointer<const float32, 16>(&other.array[12]));

	SIMD128<float32> Bro1, Bro2, Bro3, Bro4, Row;

	for (uint8 i = 0; i < 4; ++i)
	{
		Bro1 = array[4 * i + 0]; Bro2 = array[4 * i + 1]; Bro3 = array[4 * i + 2]; Bro4 = array[4 * i + 3];

		Row = Bro1 * Row1 + Bro2 * Row2 + Bro3 * Row3 + Bro4 * Row4;

		Row.CopyTo(AlignedPointer<float32, 16>(&Result.array[4 * i]));
	}

	return Result;
}

Matrix4& Matrix4::operator*=(const float32 other)
{
	const auto brod = SIMD128<float32>(other);
	
	for(uint8 i = 0; i < 4; ++i)
	{
		auto row = SIMD128<float32>(AlignedPointer<const float32, 16>(&array[i * 4]));
		row *= brod;
		row.CopyTo(AlignedPointer<float32, 16>(&array[i * 4]));
	}

	return *this;
}

Matrix4& Matrix4::operator*=(const Matrix4& other)
{
	//https://codereview.stackexchange.com/questions/101144/simd-matrix-multiplication
	
	const auto other_row_1 = SIMD128<float32>(AlignedPointer<const float32, 16>(&other.array[0]));
	const auto other_row_2 = SIMD128<float32>(AlignedPointer<const float32, 16>(&other.array[4]));
	const auto other_row_3 = SIMD128<float32>(AlignedPointer<const float32, 16>(&other.array[8]));
	const auto other_row_4 = SIMD128<float32>(AlignedPointer<const float32, 16>(&other.array[12]));

	SIMD128<float32> this_x, this_y, this_z, this_w, row;

	for (uint8 i = 0; i < 4; ++i)
	{
		this_x = array[4 * i + 0]; this_y = array[4 * i + 1]; this_z = array[4 * i + 2]; this_w = array[4 * i + 3];

		row = this_x * other_row_1 + this_y * other_row_2 + this_z * other_row_3 + this_w * other_row_4;

		row.CopyTo(AlignedPointer<float32, 16>(&array[4 * i]));
	}

	return *this;
}
