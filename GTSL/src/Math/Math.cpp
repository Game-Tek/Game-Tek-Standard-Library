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

//float32 Math::DotProduct(const Vector2& _A, const Vector2& _B)
//{
//	return SIMD128<float32>::DotProduct(SIMD128<float32>(_A.X, _A.Y, 0.0f, 0.0f), SIMD128<float32>(_B.X, _B.Y, 0.0f, 0.0f)).GetElement<0>();
//}

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

#define MakeShuffleMask(x,y,z,w)           (x | (y<<2) | (z<<4) | (w<<6))

// vec(0, 1, 2, 3) -> (vec[x], vec[y], vec[z], vec[w])
#define VecSwizzleMask(vec, mask)          _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(vec), mask))
#define VecSwizzle(vec, x, y, z, w)        VecSwizzleMask(vec, MakeShuffleMask(x,y,z,w))
#define VecSwizzle1(vec, x)                VecSwizzleMask(vec, MakeShuffleMask(x,x,x,x))
// special swizzle
#define VecSwizzle_0022(vec)               _mm_moveldup_ps(vec)
#define VecSwizzle_1133(vec)               _mm_movehdup_ps(vec)

// return (vec1[x], vec1[y], vec2[z], vec2[w])
#define VecShuffle(vec1, vec2, x,y,z,w)    _mm_shuffle_ps(vec1, vec2, MakeShuffleMask(x,y,z,w))
// special shuffle
#define VecShuffle_0101(vec1, vec2)        _mm_movelh_ps(vec1, vec2)
#define VecShuffle_2323(vec1, vec2)        _mm_movehl_ps(vec2, vec1)

// for row major matrix
// we use __m128 to represent 2x2 matrix as A = | A0  A1 |
//                                              | A2  A3 |
// 2x2 row major Matrix multiply A*B
__forceinline __m128 Mat2Mul(__m128 vec1, __m128 vec2)
{
	return
		_mm_add_ps(_mm_mul_ps(vec1, VecSwizzle(vec2, 0, 3, 0, 3)),
			_mm_mul_ps(VecSwizzle(vec1, 1, 0, 3, 2), VecSwizzle(vec2, 2, 1, 2, 1)));
}
// 2x2 row major Matrix adjugate multiply (A#)*B
__forceinline __m128 Mat2AdjMul(__m128 vec1, __m128 vec2)
{
	return
		_mm_sub_ps(_mm_mul_ps(VecSwizzle(vec1, 3, 3, 0, 0), vec2),
			_mm_mul_ps(VecSwizzle(vec1, 1, 1, 2, 2), VecSwizzle(vec2, 2, 3, 0, 1)));

}
// 2x2 row major Matrix multiply adjugate A*(B#)
__forceinline __m128 Mat2MulAdj(__m128 vec1, __m128 vec2)
{
	return
		_mm_sub_ps(_mm_mul_ps(vec1, VecSwizzle(vec2, 3, 0, 3, 0)),
			_mm_mul_ps(VecSwizzle(vec1, 1, 0, 3, 2), VecSwizzle(vec2, 2, 1, 2, 1)));
}

Matrix4 Math::Inverse(const Matrix4& matrix)
{
	// use block matrix method
	// A is a matrix, then i(A) or iA means inverse of A, A# (or A_ in code) means adjugate of A, |A| (or detA in code) is determinant, tr(A) is trace

	// sub matrices
	auto vec0 = _mm_load_ps(matrix.GetData(0, 0));
	auto vec1 = _mm_load_ps(matrix.GetData(1, 0));
	auto vec2 = _mm_load_ps(matrix.GetData(2, 0));
	auto vec3 = _mm_load_ps(matrix.GetData(3, 0));
	
	__m128 A = VecShuffle_0101(vec0, vec1);
	__m128 B = VecShuffle_2323(vec0, vec1);
	__m128 C = VecShuffle_0101(vec2, vec3);
	__m128 D = VecShuffle_2323(vec2, vec3);

	// determinant as (|A| |B| |C| |D|)
	__m128 detSub = _mm_sub_ps(
		_mm_mul_ps(VecShuffle(vec0, vec2, 0, 2, 0, 2), VecShuffle(vec1, vec3, 1, 3, 1, 3)),
		_mm_mul_ps(VecShuffle(vec0, vec2, 1, 3, 1, 3), VecShuffle(vec1, vec3, 0, 2, 0, 2))
	);
	__m128 detA = VecSwizzle1(detSub, 0);
	__m128 detB = VecSwizzle1(detSub, 1);
	__m128 detC = VecSwizzle1(detSub, 2);
	__m128 detD = VecSwizzle1(detSub, 3);

	// let iM = 1/|M| * | X  Y |
	//                  | Z  W |

	// D#C
	__m128 D_C = Mat2AdjMul(D, C);
	// A#B
	__m128 A_B = Mat2AdjMul(A, B);
	// X# = |D|A - B(D#C)
	__m128 X_ = _mm_sub_ps(_mm_mul_ps(detD, A), Mat2Mul(B, D_C));
	// W# = |A|D - C(A#B)
	__m128 W_ = _mm_sub_ps(_mm_mul_ps(detA, D), Mat2Mul(C, A_B));

	// |M| = |A|*|D| + ... (continue later)
	__m128 detM = _mm_mul_ps(detA, detD);

	// Y# = |B|C - D(A#B)#
	__m128 Y_ = _mm_sub_ps(_mm_mul_ps(detB, C), Mat2MulAdj(D, A_B));
	// Z# = |C|B - A(D#C)#
	__m128 Z_ = _mm_sub_ps(_mm_mul_ps(detC, B), Mat2MulAdj(A, D_C));

	// |M| = |A|*|D| + |B|*|C| ... (continue later)
	detM = _mm_add_ps(detM, _mm_mul_ps(detB, detC));

	// tr((A#B)(D#C))
	__m128 tr = _mm_mul_ps(A_B, VecSwizzle(D_C, 0, 2, 1, 3));
	tr = _mm_hadd_ps(tr, tr);
	tr = _mm_hadd_ps(tr, tr);
	// |M| = |A|*|D| + |B|*|C| - tr((A#B)(D#C)
	detM = _mm_sub_ps(detM, tr);

	const __m128 adjSignMask = _mm_setr_ps(1.f, -1.f, -1.f, 1.f);
	// (1/|M|, -1/|M|, -1/|M|, 1/|M|)
	__m128 rDetM = _mm_div_ps(adjSignMask, detM);

	X_ = _mm_mul_ps(X_, rDetM);
	Y_ = _mm_mul_ps(Y_, rDetM);
	Z_ = _mm_mul_ps(Z_, rDetM);
	W_ = _mm_mul_ps(W_, rDetM);

	Matrix4 result;
	
	// apply adjugate and store, here we combine adjugate shuffle and store shuffle
	_mm_store_ps(result.GetData(0, 0), VecShuffle(X_, Y_, 3, 1, 3, 1));
	_mm_store_ps(result.GetData(1, 0), VecShuffle(X_, Y_, 2, 0, 2, 0));
	_mm_store_ps(result.GetData(2, 0), VecShuffle(Z_, W_, 3, 1, 3, 1));
	_mm_store_ps(result.GetData(3, 0), VecShuffle(Z_, W_, 2, 0, 2, 0));

	return result;
}
