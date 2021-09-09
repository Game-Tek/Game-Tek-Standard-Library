#include "GTSL/Math/AxisAngle.h"

#include "GTSL/Math/Math.hpp"
#include "GTSL/SIMD/SIMD.hpp"

using namespace GTSL;

AxisAngle::AxisAngle(const Vector3& vector, const float32 angle) : X(vector.X()), Y(vector.Y()), Z(vector.Z()), Angle(angle)
{
}

AxisAngle::AxisAngle(const Quaternion& quaternion) : Angle(2.0f * Math::ArcCosine(quaternion.W()))
{
	float4x components(quaternion.X(), quaternion.Y(), quaternion.Z(), quaternion.W());
	const float4x sqrt(1 - quaternion.W() * quaternion.W());
	components /= sqrt;
	alignas(16) float data[4];
	components.CopyTo(AlignedPointer<float32, 16>(data));
	X = data[0];
	Y = data[1];
	Z = data[2];
}

AxisAngle AxisAngle::Normalized(const AxisAngle& axisAngle)
{
	const float32 mag = Math::Length(Vector3(axisAngle.X, axisAngle.Y, axisAngle.Z));
	return { axisAngle.X / mag, axisAngle.Y / mag, axisAngle.Z / mag, axisAngle.Angle };
}
