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

	//X() = sy * cp * cr - cy * sp * sr;
	//Y() = sy * cp * sr + cy * sp * cr;

	Y() = sy * cp * cr - cy * sp * sr;
	X() = sy * cp * sr + cy * sp * cr;
	
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
	W() = Math::Cosine(-axisAngle.Angle / 2);
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

Vector4 Quaternion::GetXBasisVector() const
{
	//return (*this) * GTSL::Math::Right;
	return GTSL::Matrix4(*this).GetXBasisVector();
}

Vector4 Quaternion::GetYBasisVector() const
{
	return (*this) * GTSL::Math::Up;
}

Vector4 Quaternion::GetZBasisVector() const
{
	return (*this) * GTSL::Math::Forward;
}

//stack overflow?

//Quaternion& Quaternion::operator*=(const Quaternion& other)
//{
//	X() = W() * other.X() + X() * other.W() + Y() * other.Z() - Z() * other.Y();
//	Y() = W() * other.Y() + Y() * other.W() + Z() * other.X() - X() * other.Z();
//	Z() = W() * other.Z() + Z() * other.W() + X() * other.Y() - Y() * other.X();
//	W() = W() * other.W() - X() * other.X() - Y() * other.Y() - Z() * other.Z();
//	
//	return *this;
//}

//public final void mul(Quat4d q1, Quat4d q2) {
//	x   = q1.x  * q2.w + q1.y * q2.z - q1.z * q2.y + q1.w * q2.x;
//	y   = -q1.x * q2.z + q1.y * q2.w + q1.z * q2.x + q1.w * q2.y;
//	z   = q1.x  * q2.y - q1.y * q2.x + q1.z * q2.w + q1.w * q2.z;
//	w   = -q1.x * q2.x - q1.y * q2.y - q1.z * q2.z + q1.w * q2.w;
//}

//https://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/code/index.htm

Quaternion& Quaternion::operator*=(const Quaternion& other)
{
	X() = X()  * other.W() + Y() * other.Z() - Z() * other.Y() + W() * other.X();
	Y() = -X() * other.Z() + Y() * other.W() + Z() * other.X() + W() * other.Y();
	Z() = X()  * other.Y() - Y() * other.X() + Z() * other.W() + W() * other.Z();
	W() = -X() * other.X() - Y() * other.Y() - Z() * other.Z() + W() * other.W();
	
	return *this;
}
