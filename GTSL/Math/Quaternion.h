#pragma once

#include "Vectors.hpp"

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

		Quaternion& operator*=(const Quaternion& other) {
			auto x = X() * other.W() + Y() * other.Z() - Z() * other.Y() + W() * other.X();
			auto y = -X() * other.Z() + Y() * other.W() + Z() * other.X() + W() * other.Y();
			auto z = X() * other.Y() - Y() * other.X() + Z() * other.W() + W() * other.Z();
			auto w = -X() * other.X() - Y() * other.Y() - Z() * other.Z() + W() * other.W();
			X() = x; Y() = y; Z() = z; W() = w;
			return *this;
		}

		Quaternion operator*(const Quaternion& other) const {
			Quaternion result(*this);
			return result *= other;
		}

		Vector3 operator*(const Vector3 other) const;

		Vector4 GetXBasisVector() const;
		Vector4 GetYBasisVector() const;
		Vector4 GetZBasisVector() const;
	};
}
