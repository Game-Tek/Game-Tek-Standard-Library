#pragma once

#include "GTSL/Core.h"

namespace GTSL
{
	class Vector4;

	//Used to specify a location in 3D space with floating point precision.
	class Vector3
	{
	public:
		//X component of this vector.
		float32 X = 0.0f;

		//Y component of this vector.
		float32 Y = 0.0f;

		//Z component of this vector.
		float32 Z = 0.0f;


		Vector3() = default;

		Vector3(const float32 X, const float32 Y, const float32 Z) : X(X), Y(Y), Z(Z)
		{
		}

		explicit Vector3(const class Rotator& rotator);
		
		Vector3(const Vector4& vector4);

		Vector3(const Vector3& other) = default;

		~Vector3() = default;

		Vector3 operator+(const float32 other) const
		{
			return { X + other, Y + other, Z + other };
		}

		Vector3 operator+(const Vector3& other) const
		{
			return { X + other.X, Y + other.Y, Z + other.Z };
		}

		Vector3& operator+=(const float32 other)
		{
			X += other; Y += other; Z += other;

			return *this;
		}

		Vector3& operator+=(const Vector3& other)
		{
			X += other.X;
			Y += other.Y;
			Z += other.Z;

			return *this;
		}

		Vector3 operator-(const float32 other) const
		{
			return { X - other, Y - other, Z - other };
		}

		Vector3 operator-(const Vector3& other) const
		{
			return { X - other.X, Y - other.Y, Z - other.Z };
		}

		Vector3& operator-=(const float32 other)
		{
			X -= other;
			Y -= other;
			Z -= other;

			return *this;
		}

		Vector3& operator-=(const Vector3& other)
		{
			X -= other.X;
			Y -= other.Y;
			Z -= other.Z;

			return *this;
		}

		Vector3 operator*(const float32 other) const
		{
			return { X * other, Y * other, Z * other };
		}

		Vector3 operator*(const Vector3& other) const
		{
			return { X * other.X, Y * other.Y, Z * other.Z };
		}

		Vector3& operator*=(const float32 other)
		{
			X *= other;
			Y *= other;
			Z *= other;

			return *this;
		}

		Vector3& operator*=(const class Quaternion& quaternion);

		Vector3 operator/(const float32 other) const
		{
			return { X / other, Y / other, Z / other };
		}

		Vector3& operator/=(const float32 other)
		{
			X /= other;
			Y /= other;
			Z /= other;

			return *this;
		}

		bool operator==(const Vector3& other) const
		{
			return X == other.X && Y == other.Y && Z == other.Z;
		}

		bool operator!=(const Vector3& other) const
		{
			return X != other.X || Y != other.Y || Z != other.Z;
		}
	};
}