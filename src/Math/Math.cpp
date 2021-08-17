#include "GTSL/Math/Math.hpp"

#include "GTSL/SIMD/SIMD128.hpp"
#include <cmath>

using namespace GTSL;

float32 Math::Power(const float32 x, const float32 y) { return powf(x, y); }

float32 Math::Log10(const float32 x) { return log10f(x); }

float32 Math::Sine(float32 x) { return sinf(x); }
	
float64 Math::Sine(const float64 radians) { return ::sin(radians); }

float32 Math::Cosine(float32 x) { return cosf(x); }

float64 Math::Cosine(const float64 radians) { return ::cos(radians); }

float32 Math::Tangent(float32 x)
{
	return tanf(x);
}

float64 Math::Tangent(const float64 radians) { return tan(radians); }

float32 Math::ArcSine(const float32 A) {	return asin(A); }

float32 Math::ArcCosine(const float32 A) { return acos(A); }

float32 Math::ArcTangent(const float32 A) { return atan(A); }

float32 Math::ArcTan2(const float32 y, const float32 x) { return atan2(y, x); }