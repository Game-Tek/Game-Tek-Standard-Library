#pragma once

#include "Vector4.h"

#include "Vector3.h"

namespace GTSL
{
	class AxisAngle;
	class Rotator;

	class Quaternion : public Vector4
	{
	public:
		Quaternion() = default;
		Quaternion(const float32 x, const float32 y, const float32 z, const float32 w) : Vector4(x, y, z, w) {}

		explicit Quaternion(const Rotator& rotator);
		explicit Quaternion(const AxisAngle& axisAngle);

		Quaternion& operator*=(const Quaternion& other);
		Quaternion operator*(const Quaternion& other) const;
		Vector3 operator*(const Vector3 other) const;

		operator Vector4() const { return *this; }
		operator Vector4&() { return *this; }
		operator const Vector4&() const { return *this; }
	};
}
