#pragma once

#include "Vectors.h"

namespace GTSL
{
	class AxisAngle;
	class Rotator;

	class Quaternion : public Vector4
	{
	public:
		Quaternion() : Vector4(0, 0, 0, 1) {}
		Quaternion(const float32 x, const float32 y, const float32 z, const float32 w) : Vector4(x, y, z, w) {}

		explicit Quaternion(const Rotator& rotator);
		explicit Quaternion(const AxisAngle& axisAngle);

		Quaternion& operator*=(const Quaternion& other);
		Quaternion operator*(const Quaternion& other) const;
		Vector3 operator*(const Vector3 other) const;

		Vector4 GetXBasisVector() const;
		Vector4 GetYBasisVector() const;
		Vector4 GetZBasisVector() const;
		
		operator Vector4() const { return *this; }
		operator Vector4&() { return *this; }
		operator const Vector4&() const { return *this; }
	};
}
