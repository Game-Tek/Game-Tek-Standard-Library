#pragma once

#include "GTSL/Core.h"

namespace GTSL
{
	class Vector3;

	class alignas(16) Vector4
	{
	public:
		Vector4() = default;

		Vector4(const float32 x, const float32 y, const float32 z, const float32 w) : values{ x, y, z, w } {}

		Vector4(const Vector3& vector3);
		Vector4(const Vector3& vector3, const float32 w);
		
		~Vector4() = default;

		Vector4 operator+(const float other) const
		{
			return { X() + other, Y() + other, Z() + other, W() + other };
		}

		Vector4 operator+(const Vector4& other) const
		{
			return { X() + other.X(), Y() + other.Y(), Z() + other.Z(), W() + other.W() };
		}

		Vector4& operator+=(float Other)
		{
			X() += Other; Y() += Other; Z() += Other; W() += Other;

			return *this;
		}

		Vector4& operator+=(const Vector4& Other)
		{
			X() += Other.X(); Y() += Other.Y(); Z() += Other.Z(); W() += Other.W();

			return *this;
		}

		Vector4 operator-(float Other) const
		{
			return { X() - Other, Y() - Other, Z() - Other, W() - Other };
		}

		Vector4 operator-(const Vector4& Other) const
		{
			return { X() - Other.X(), Y() - Other.Y(), Z() - Other.Z(), W() - Other.W() };
		}

		Vector4& operator-=(float Other)
		{
			X() -= Other; Y() -= Other; Z() -= Other; W() -= Other;

			return *this;
		}

		Vector4& operator-=(const Vector4& Other)
		{
			X() -= Other.X(); Y() -= Other.Y(); Z() -= Other.Z(); W() -= Other.W();

			return *this;
		}

		Vector4 operator*(float32 other) const
		{
			return { X() * other, Y() * other, Z() * other, W() * other };
		}

		Vector4& operator*=(float32 other)
		{
			X() *= other; Y() *= other; Z() *= other; W() *= other;
			return *this;
		}

		Vector4 operator/(float32 other) const
		{
			return { X() / other, Y() / other, Z() / other, W() / other };
		}

		Vector4& operator/=(float32 other)
		{
			X() /= other; Y() /= other; Z() /= other; W() /= other;

			return *this;
		}

		bool operator==(const Vector4& Other) { return X() == Other.X() && Y() == Other.Y() && Z() == Other.Z() && W() == Other.W(); }

		bool operator!=(const Vector4& Other) { return X() != Other.X() || Y() != Other.Y() || Z() != Other.Z() || W() != Other.W(); }

		float32& X() { return values[0]; }
		float32& Y() { return values[1]; }
		float32& Z() { return values[2]; }
		float32& W() { return values[3]; }

		float32 X() const { return values[0]; }
		float32 Y() const { return values[1]; }
		float32 Z() const { return values[2]; }
		float32 W() const { return values[3]; }

		float32* GetData() { return values; }
		const float32* GetData() const { return values; }

		float32 operator[](const uint8 i) const { return values[i]; }
		float32& operator[](const uint8 i) { return values[i]; }
		
	private:
		float32 values[4]{ 0.0f };
	};
}