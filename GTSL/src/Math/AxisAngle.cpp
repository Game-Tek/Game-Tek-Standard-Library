#include "GTSL/Math/AxisAngle.h"

#include "GTSL/Math/Math.hpp"
#include "GTSL/SIMD/float32_4.h"

using namespace GTSL;

constexpr AxisAngle::AxisAngle(const Vector3& vector, const float angle) :
	X(vector.X), Y(vector.Y), Z(vector.Z), Angle(angle)
{
}

AxisAngle::AxisAngle(const Quaternion& quaternion) : Angle(2.0f * Math::ArcCosine(quaternion.Q))
{
	float32_4 components(quaternion.X, quaternion.Y, quaternion.Z, quaternion.Q);
	float32_4 sqrt(1 - quaternion.Q * quaternion.Q);
	components /= sqrt;
	alignas(16) float data[4];
	components.CopyToAlignedData(data);
	X = data[0];
	Y = data[1];
	Z = data[2];
}
