#include "GTSL/Math/Quaternion.h"

#include "GTSL/Math/Math.hpp"
#include "GTSL/Math/AxisAngle.h"

using namespace GTSL;

//CODE IS CORRECT
Quaternion::Quaternion(const Rotator& rotator)
{
	// Abbreviations for the various angular functions
	const auto cy = Math::Cosine(rotator.Y * 0.5f);
	const auto sy = Math::Sine(rotator.Y   * 0.5f);
	const auto cp = Math::Cosine(rotator.X * 0.5f);
	const auto sp = Math::Sine(rotator.X   * 0.5f);
	const auto cr = Math::Cosine(rotator.Z * 0.5f);
	const auto sr = Math::Sine(rotator.Z   * 0.5f);

	X() = sy * cp * cr - cy * sp * sr;
	Y() = sy * cp * sr + cy * sp * cr;
	Z() = cy * cp * sr - sy * sp * cr;
	W() = cy * cp * cr + sy * sp * sr;
}

//Quaternion::Quaternion(const AxisAngle& axisAngle) : Vector4(axisAngle.X * Math::Sine(axisAngle.Angle / 2), axisAngle.Y * Math::Sine(axisAngle.Angle / 2), axisAngle.Z * Math::Sine(axisAngle.Angle / 2), Math::Cosine(axisAngle.Angle / 2))
//{
//}

Quaternion::Quaternion(const AxisAngle& axisAngle)
{
	auto halfAngleSine = Math::Sine(axisAngle.Angle / 2);
	X() = axisAngle.X * halfAngleSine;
	Y() = axisAngle.Y * halfAngleSine;
	Z() = axisAngle.Z * halfAngleSine;
	W() = Math::Cosine(axisAngle.Angle / 2);
}

Quaternion Quaternion::operator*(const Quaternion& other) const
{
	Quaternion result;

	return result *= other;
}

Vector3 Quaternion::operator*(const Vector3 other) const
{
	Vector3 u(X(), Y(), Z()); float32 s = W();
	return u * 2.0f * Math::DotProduct(u, other) + other * (s * s - Math::DotProduct(u, u)) + Math::Cross(u, other) * 2.0f * s;
}

Quaternion& Quaternion::operator*=(const Quaternion& other)
{
	X() = W() * other.X() + X() * other.W() + Y() * other.Z() - Z() * other.Y();
	Y() = W() * other.Y() + Y() * other.W() + Z() * other.X() - X() * other.Z();
	Z() = W() * other.Z() + Z() * other.W() + X() * other.Y() - Y() * other.X();
	W() = W() * other.W() - X() * other.X() - Y() * other.Y() - Z() * other.Z();
	
	return *this;
}
