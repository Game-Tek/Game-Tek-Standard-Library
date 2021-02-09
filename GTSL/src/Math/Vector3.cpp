#include "GTSL/Math/Math.hpp"

using namespace GTSL;

Vector3::Vector3(const Rotator& rotator) : Vector3(Math::Cosine(rotator.X) * Math::Sine(rotator.Y), Math::Sine(rotator.X), Math::Cosine(rotator.X) * Math::Cosine(rotator.Y))
{
	//CODE IS CORRECT
}

Vector3::Vector3(const Vector4& vector4) : Vector3(vector4.X(), vector4.Y(), vector4.Z())
{
}

Vector3 operator*(const float& lhs, const Vector3& rhs)
{
	return Vector3(rhs.X() * lhs, rhs.Y() * lhs, rhs.Z() * lhs);
}

Vector3& Vector3::operator*=(const Quaternion& quaternion)
{
	// Extract the vector part of the quaternion
	const Vector3 u(quaternion.X(), quaternion.Y(), quaternion.Z());

	// Extract the scalar part of the quaternion
	const float s = quaternion.W();

	// Do the math
	*this = u * 2.0f * Math::DotProduct(u, *this) + (s * s - Math::DotProduct(u, u)) * *this + 2.0f * s * Math::Cross(u, *this);

	return *this;
}
