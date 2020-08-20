#include "GTSL/Math/Math.hpp"

#include "GTSL/SIMD/SIMD128.hpp"
#include <cmath>

using namespace GTSL;

float32 Math::Power(const float32 x, const float32 y) { return powf(x, y); }

float32 Math::Log10(const float32 x) { return log10f(x); }

float32 Math::Sine(const float32 radians) { return sinf(radians); }

float64 Math::Sine(const float64 radians) { return sin(radians); }

float32 Math::Cosine(const float32 radians) { return cosf(radians); }

float64 Math::Cosine(const float64 radians) { return cos(radians); }

float32 Math::Tangent(const float32 radians) { return tanf(radians); }

float64 Math::Tangent(const float64 radians) { return tan(radians); }

float32 Math::ArcSine(const float32 A) {	return asin(A); }

float32 Math::ArcCosine(const float32 A) { return acos(A); }

float32 Math::ArcTangent(const float32 A) { return atan(A); }

float32 Math::ArcTan2(const float32 X, const float32 Y) { return atan2(Y, X); }

float32 Math::LengthSquared(const Vector2& a)
{
	const SIMD128<float32> vec(a.X, a.Y, 0.0f, 0.0f);
	return SIMD128<float32>::DotProduct(vec, vec).GetElement<0>();
}

float32 Math::LengthSquared(const Vector3& a)
{
	//const SIMD128<float32> vec(a.X, a.Y, a.Z, 0.0f);
	//return SIMD128<float32>::DotProduct(vec, vec).GetElement<0>();
	//
	return a.X * a.X + a.Y * a.Y + a.Z * a.Z;
}

float32 Math::LengthSquared(const Vector4& a)
{
	const SIMD128<float32> vec(AlignedPointer<const float32, 16>(&a.X));
	return SIMD128<float32>::DotProduct(vec, vec).GetElement<0>();
}

Vector2 Math::Normalized(const Vector2& a)
{
	SIMD128<float32> vec(a.X, a.Y, 0.0f, 0.0f);
	const SIMD128<float32> length(Length(a));
	vec /= length;
	alignas(16) float32 vector[4];
	vec.CopyTo(AlignedPointer<float32, 16>(vector));

	return Vector2(vector[3], vector[2]);
}

void Math::Normalize(Vector2& a)
{
	SIMD128<float32> vec(a.X, a.Y, 0.0f, 0.0f);
	const SIMD128<float32> length(Length(a));
	vec /= length;
	alignas(16) float32 vector[4];
	vec.CopyTo(AlignedPointer<float32, 16>(vector));
	a.X = vector[3];
	a.Y = vector[2];
}

Vector3 Math::Normalized(const Vector3& _A)
{
	SIMD128<float32> vec(_A.X, _A.Y, _A.Z, 0.0f);
	const SIMD128<float32> length(Length(_A));
	alignas(16) float32 vector[4];
	vec /= length;
	vec.CopyTo(AlignedPointer<float32, 16>(vector));

	return Vector3(vector[3], vector[2], vector[1]);
}

void Math::Normalize(Vector3& a)
{
	SIMD128<float32> vec(a.X, a.Y, a.Z, 0.0f);
	const SIMD128<float32> length(Length(a));
	vec /= length;
	alignas(16) float32 vector[4];
	vec.CopyTo(AlignedPointer<float32, 16>(vector));
	a.X = vector[3];
	a.Y = vector[2];
	a.Z = vector[1];
}

Vector4 Math::Normalized(const Vector4& a)
{
	alignas(16) Vector4 result;
	auto vec = SIMD128<float32>(AlignedPointer<const float32, 16>(&a.X));
	const SIMD128<float32> length(Length(a));
	vec /= length;
	vec.CopyTo(AlignedPointer<float32, 16>(&result.X));
	return result;
}

void Math::Normalize(Vector4& a)
{
	auto vec = SIMD128<float32>(AlignedPointer<const float32, 16>(&a.X)); const SIMD128<float32> length(Length(a));
	vec /= length;
	vec.CopyTo(AlignedPointer<float32, 16>(&a.X));
}

float32 Math::DotProduct(const Vector2& _A, const Vector2& _B)
{
	return SIMD128<float32>::DotProduct(SIMD128<float32>(_A.X, _A.Y, 0.0f, 0.0f), SIMD128<float32>(_B.X, _B.Y, 0.0f, 0.0f)).GetElement<0>();
}

float32 Math::DotProduct(const Vector3& _A, const Vector3& _B)
{
	return SIMD128<float32>::DotProduct(SIMD128<float32>(_A.X, _A.Y, _A.Z, 0.0f), SIMD128<float32>(_B.X, _B.Y, _B.Z, 0.0f)).GetElement<0>();
}

float32 Math::DotProduct(const Vector4& _A, const Vector4& _B)
{
	return SIMD128<float32>::DotProduct(SIMD128<float32>(_A.X, _A.Y, _A.Z, _A.W), SIMD128<float32>(_B.X, _B.Y, _B.Z, _A.W)).GetElement<0>();
}

Vector3 Math::Cross(const Vector3& a, const Vector3& b)
{
	//alignas(16) float32 vector[4];
	//
	//const float4 a(_A.X, _A.Y, _A.Z, 1.0f);
	//const float4 b(_B.X, _B.Y, _B.Z, 1.0f);
	//
	//const float4 res = float4::Shuffle<3, 0, 2, 1>(a, a) * float4::Shuffle<3, 1, 0, 2>(b, b) - float4::Shuffle<3, 0, 2, 1>(a, a) * float4::Shuffle<3, 0, 2, 1>(b, b);
	//res.CopyToAlignedData(vector);
	//
	//return Vector3(vector[3], vector[2], vector[1]);
	//

	return Vector3(a.Y * b.Z - a.Z * b.Y, a.Z * b.X - a.X * b.Z, a.X * b.Y - a.Y * b.X);
}

Vector4 Math::Abs(const Vector4& a)
{
	SIMD128<float32> vec(AlignedPointer<const float32, 16>(&a.X));
	vec.Abs();
	Vector4 result;
	vec.CopyTo(AlignedPointer<float32, 16>(&result.X));
	return result;
}

float32 Math::DotProduct(const Quaternion& a, const Quaternion& b)
{
	return SIMD128<float32>::DotProduct(SIMD128<float32>(a.X, a.Y, a.Z, a.Q), SIMD128<float32>(b.X, b.Y, b.Z, a.Q)).GetElement<0>();
}

float32 Math::LengthSquared(const Quaternion& a)
{
	const SIMD128<float32> vec(AlignedPointer<const float32, 16>(&a.X));
	return SIMD128<float32>::DotProduct(vec, vec).GetElement<0>();
}

Quaternion Math::Normalized(const Quaternion& a)
{
	alignas(16) Quaternion result;
	auto vec = SIMD128<float32>(AlignedPointer<const float32, 16>(&a.X));
	const SIMD128<float32> length(Length(a));
	vec /= length;
	vec.CopyTo(AlignedPointer<float32, 16>(&result.X));
	return result;
}

void Math::Normalize(Quaternion& a)
{
	auto vec = SIMD128<float32>(AlignedPointer<const float32, 16>(&a.X));
	const SIMD128<float32> length(Length(a));
	vec /= length;
	vec.CopyTo(AlignedPointer<float32, 16>(&a.X));
}
