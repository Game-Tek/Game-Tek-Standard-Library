#pragma once

#include "Vector4.h"

namespace GTSL
{
	class AxisAngle;
	class Rotator;

	class Quaternion : public Vector4
	{
	public:
		Quaternion() = default;
		Quaternion(const float32 x, const float32 y, const float32 z, const float32 w) : Vector4(x, y, z, w) {}
		
		Quaternion(const Rotator& rotator);
		Quaternion(const AxisAngle& axisAngle);

		Quaternion& operator*=(const Quaternion& other);
		Quaternion operator*(const Quaternion& other) const;

		operator Vector4() const { return *this; }
		operator Vector4&() { return *this; }
		operator const Vector4&() const { return *this; }
	};
}
