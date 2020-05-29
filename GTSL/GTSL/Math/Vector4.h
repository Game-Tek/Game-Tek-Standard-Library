#pragma once

#include "GTSL/Core.h"

namespace GTSL
{
	class alignas(16) Vector4
	{
	public:
		//X component of this vector.
		float32 X = 0.0f;

		//Y component of this vector.
		float32 Y = 0.0f;

		//Z component of this vector.
		float32 Z = 0.0f;

		//W component of this vector.
		float32 W = 0.0f;

		Vector4() = default;

		Vector4(const float32 x, const float32 y, const float32 z, const float32 w) : X(x), Y(y), Z(z), W(w) {}

		~Vector4() = default;

		Vector4 operator+(const float other) const
		{
			return { X + other, Y + other, Z + other, W + other };
		}

		Vector4 operator+(const Vector4& other) const
		{
			return { X + other.X, Y + other.Y, Z + other.Z, W + other.W };
		}

		Vector4& operator+=(float Other)
		{
			X += Other;
			Y += Other;
			Z += Other;
			W += Other;

			return *this;
		}

		Vector4& operator+=(const Vector4& Other)
		{
			X += Other.X;
			Y += Other.Y;
			Z += Other.Z;
			W += Other.W;

			return *this;
		}

		Vector4 operator-(float Other) const
		{
			return { X - Other, Y - Other, Z - Other, W - Other };
		}

		Vector4 operator-(const Vector4& Other) const
		{
			return { X - Other.X, Y - Other.Y, Z - Other.Z, W - Other.W };
		}

		Vector4& operator-=(float Other)
		{
			X -= Other;
			Y -= Other;
			Z -= Other;
			W -= Other;

			return *this;
		}

		Vector4& operator-=(const Vector4& Other)
		{
			X -= Other.X;
			Y -= Other.Y;
			Z -= Other.Z;
			W -= Other.W;

			return *this;
		}

		Vector4 operator*(float Other) const
		{
			return { X * Other, Y * Other, Z * Other, W * Other };
		}

		Vector4& operator*=(float Other)
		{
			X *= Other;
			Y *= Other;
			Z *= Other;
			W *= Other;

			return *this;
		}

		Vector4 operator/(float Other) const
		{
			return { X / Other, Y / Other, Z / Other, W / Other };
		}

		Vector4& operator/=(float Other)
		{
			X /= Other;
			Y /= Other;
			Z /= Other;
			W /= Other;

			return *this;
		}

		bool operator==(const Vector4& Other) { return X == Other.X && Y == Other.Y && Z == Other.Z && W == Other.W; }

		bool operator!=(const Vector4& Other) { return X != Other.X || Y != Other.Y || Z != Other.Z || W != Other.W; }
	};
}