#include "GTSL/Math/Math.hpp"

#include "GTSL/SIMD/SIMD128.h"
#include <cmath>

using namespace GTSL;

float Math::Power(const float32 x, const float32 y) { return powf(x, y); }

float Math::Log10(const float32 x) { return log10f(x); }

float Math::Sine(const float32 Degrees) { return sinf(DegreesToRadians(Degrees)); }

double Math::Sine(const float64 Degrees) { return sin(DegreesToRadians(Degrees)); }

float Math::Cosine(const float32 Degrees) { return cosf(DegreesToRadians(Degrees)); }

double Math::Cosine(const float64 Degrees) { return cos(DegreesToRadians(Degrees)); }

float Math::Tangent(const float32 Degrees) { return tanf(DegreesToRadians(Degrees)); }

double Math::Tangent(const float64 Degrees) { return tan(DegreesToRadians(Degrees)); }

float Math::ArcSine(const float32 A) {	return RadiansToDegrees(asin(A)); }

float Math::ArcCosine(const float32 A) { return RadiansToDegrees(acos(A)); }

float Math::ArcTangent(const float32 A) { return RadiansToDegrees(atan(A)); }

float Math::ArcTan2(const float32 X, const float32 Y) { return RadiansToDegrees(atan2(Y, X)); }

float Math::LengthSquared(const Vector2& _A)
{
	const SIMD128<float32> a(_A.X, _A.Y, 0.0f, 0.0f);
	return SIMD128<float32>::DotProduct(a, a).GetElement<1>();
}

float Math::LengthSquared(const Vector3& _A)
{
	const SIMD128<float32> a(_A.X, _A.Y, _A.Z, 0.0f);
	return SIMD128<float32>::DotProduct(a, a).GetElement<1>();
}

float Math::LengthSquared(const Vector4& _A)
{
	const SIMD128<float32> a(AlignedPointer<float32, 16>(&_A.X));
	return SIMD128<float32>::DotProduct(a, a).GetElement<1>();
}

Vector2 Math::Normalized(const Vector2& _A)
{
	SIMD128<float32> a(_A.X, _A.Y, 0.0f, 0.0f);
	const SIMD128<float32> length(Length(_A));
	a /= length;
	alignas(16) float vector[4];
	a.CopyTo(AlignedPointer<float, 16>(vector));

	return Vector2(vector[3], vector[2]);
}

void Math::Normalize(Vector2& _A)
{
	SIMD128<float32> a(_A.X, _A.Y, 0.0f, 0.0f);
	const SIMD128<float32> length(Length(_A));
	a /= length;
	alignas(16) float vector[4];
	a.CopyTo(AlignedPointer<float, 16>(vector));
	_A.X = vector[3];
	_A.Y = vector[2];
}

Vector3 Math::Normalized(const Vector3& _A)
{
	SIMD128<float32> a(_A.X, _A.Y, _A.Z, 0.0f);
	const SIMD128<float32> length(Length(_A));
	alignas(16) float32 vector[4];
	a /= length;
	a.CopyTo(AlignedPointer<float, 16>(vector));

	return Vector3(vector[3], vector[2], vector[1]);
}

void Math::Normalize(Vector3& _A)
{
	SIMD128<float32> a(_A.X, _A.Y, _A.Z, 0.0f);
	const SIMD128<float32> length(Length(_A));
	a /= length;
	alignas(16) float vector[4];
	a.CopyTo(AlignedPointer<float32, 16>(vector));
	_A.X = vector[3];
	_A.Y = vector[2];
	_A.Z = vector[1];
}

Vector4 Math::Normalized(const Vector4& a)
{
	alignas(16) Vector4 result;
	auto vec = SIMD128<float32>(UnalignedPointer<float32>(&a.X));
	const SIMD128<float32> length(Length(a));
	vec /= length;
	vec.CopyTo(AlignedPointer<float32, 16>(&result.X));
	return result;
}

void Math::Normalize(Vector4& _A)
{
	auto a = SIMD128<float32>(UnalignedPointer<float32>(&_A.X)); const SIMD128<float32> length(Length(_A));
	a /= length;
	a.CopyTo(UnalignedPointer<float32>(&_A.X));
}

float Math::DotProduct(const Vector2& _A, const Vector2& _B)
{
	return SIMD128<float32>::DotProduct(SIMD128<float32>(_A.X, _A.Y, 0.0f, 0.0f), SIMD128<float32>(_B.X, _B.Y, 0.0f, 0.0f)).GetElement<1>();
}

float Math::DotProduct(const Vector3& _A, const Vector3& _B)
{
	return SIMD128<float32>::DotProduct(SIMD128<float32>(_A.X, _A.Y, _A.Z, 0.0f), SIMD128<float32>(_B.X, _B.Y, _B.Z, 0.0f)).GetElement<1>();
}

float Math::DotProduct(const Vector4& _A, const Vector4& _B)
{
	return SIMD128<float32>::DotProduct(SIMD128<float32>(_A.X, _A.Y, _A.Z, _A.W), SIMD128<float32>(_B.X, _B.Y, _B.Z, _A.W)).GetElement<1>();
}

Vector3 Math::Cross(const Vector3& a, const Vector3& b)
{
	//alignas(16) float vector[4];
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

float Math::DotProduct(const Quaternion& _A, const Quaternion& _B)
{
	return SIMD128<float32>::DotProduct(SIMD128<float32>(_A.X, _A.Y, _A.Z, _A.Q), SIMD128<float32>(_B.X, _B.Y, _B.Z, _A.Q)).GetElement<1>();
}

float Math::LengthSquared(const Quaternion& _A)
{
	SIMD128<float32> a(UnalignedPointer<float32>(&_A.X));
	return SIMD128<float32>::DotProduct(a, a).GetElement<1>();
}

Quaternion Math::Normalized(const Quaternion& _A)
{
	alignas(16) Quaternion result;
	auto a = SIMD128<float32>(UnalignedPointer<float32>(&_A.X));
	const SIMD128<float32> length(Length(_A));
	a /= length;
	a.CopyTo(AlignedPointer<float32, 16>(&result.X));
	return result;
}

void Math::Normalize(Quaternion& _A)
{
	auto a = SIMD128<float32>(UnalignedPointer<float32>(&_A.X));
	const SIMD128<float32> length(Length(_A));
	a /= length;
	a.CopyTo(UnalignedPointer<float32>(&_A.X));
}
