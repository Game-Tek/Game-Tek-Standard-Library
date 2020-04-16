#include "GTSL/Math/Math.hpp"

using namespace GTSL;

Vector3::Vector3(const Rotator& rotator) : X(Math::Cosine(rotator.X) * Math::Sine(rotator.Y)), Y(Math::Sine(rotator.X)), Z(Math::Cosine(rotator.X) * Math::Cosine(rotator.Y))
{
	//CODE IS CORRECT
}

Vector3 operator*(const float& lhs, const Vector3& rhs)
{
	return Vector3(rhs.X * lhs, rhs.Y * lhs, rhs.Z * lhs);
}

Vector3& Vector3::operator*=(const Quaternion& quaternion)
{
	// Extract the vector part of the quaternion
	Vector3 u(quaternion.X, quaternion.Y, quaternion.Z);

	// Extract the scalar part of the quaternion
	float s = quaternion.Q;

	// Do the math
	*this = u * 2.0f * Math::DotProduct(u, *this) + (s * s - Math::DotProduct(u, u)) * *this + 2.0f * s * Math::Cross(u, *this);

	return *this;
}
