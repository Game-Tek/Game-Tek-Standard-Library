#pragma once

namespace GTSL
{
	//TODO: Implement

	class AxisAngle
	{
	public:
		constexpr AxisAngle(const float x, const float y, const float z, const float angle)
			: X(x),Y(y),Z(z),Angle(angle) {}

		AxisAngle(const class Vector3& vector, float angle);
		explicit AxisAngle(const class Quaternion& quaternion);
		static AxisAngle Normalized(const AxisAngle& axisAngle);

		float X, Y, Z, Angle;
	};
}