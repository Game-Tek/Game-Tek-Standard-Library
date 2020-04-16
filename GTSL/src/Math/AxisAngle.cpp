#include "Math/AxisAngle.h"

#include "Math/Math.hpp"

#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "SIMD/float32_4.h"

using namespace GTSL;

constexpr AxisAngle::AxisAngle(const Vector3& vector, const float angle) :
	X(vector.X), Y(vector.Y), Z(vector.Z), Angle(angle)
{
}

AxisAngle::AxisAngle(const Quaternion& quaternion) : Angle(2.0f * Math::ArcCosine(quaternion.Q))
{
	auto components = float32_4::MakeFromUnaligned(reinterpret_cast<float*>(&const_cast<Quaternion&>(quaternion)));
	float32_4 sqrt(1 - quaternion.Q * quaternion.Q);
	components /= sqrt;
	alignas(16) float data[4];
	components.CopyToAlignedData(data);
	X = data[0];
	Y = data[1];
	Z = data[2];
}
