#pragma once

namespace GTSL
{
	class Rotator
	{
	public:
		Rotator()
		{
		}

		Rotator(float x, float y, float z) : X(x), Y(y), Z(z)
		{
		}

		Rotator(const Rotator& Other) : X(Other.X), Y(Other.Y), Z(Other.Z)
		{
		}

		explicit Rotator(const class Vector3& vector);

		Rotator operator+(const Rotator& Other) const
		{
			return { X + Other.X, Y + Other.Y, Z + Other.Z };
		}

		Rotator operator-(const Rotator& Other) const
		{
			return { X - Other.X, Y - Other.Y, Z - Other.Z };
		}

		Rotator operator*(float Other) const
		{
			return { X * Other, Y * Other, Z * Other };
		}

		Rotator operator/(float Other) const
		{
			return { X / Other, Y / Other, Z / Other };
		}

		Rotator& operator+=(const Rotator& rotator)
		{
			X += rotator.X;
			Y += rotator.Y;
			Z += rotator.Z;
			return *this;
		}

		float X, Y, Z;
	};
}