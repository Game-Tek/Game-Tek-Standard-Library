#pragma once

#include "GTSL/Core.h"

namespace GTSL
{
	class Vector4;

	//Used to specify a location in 3D space with floating point precision.
	class alignas(16) Vector3
	{
	public:
		Vector3() = default;

		constexpr Vector3(const float32 a) : values{ a, a, a, 0.0f }
		{
		}

		constexpr Vector3(const float32 x, const float32 y, const float32 z) : values{ x, y, z, 0.0f }
		{
		}

		explicit Vector3(const class Rotator& rotator);
		
		explicit Vector3(const Vector4& vector4);

		Vector3(const Vector3& other) = default;

		~Vector3() = default;

		Vector3 operator+(const float32 other) const
		{
			return { X() + other, Y() + other, Z() + other };
		}

		Vector3 operator+(const Vector3& other) const
		{
			return { X() + other.X(), Y() + other.Y(), Z() + other.Z() };
		}

		Vector3& operator+=(const float32 other)
		{
			X() += other; Y() += other; Z() += other;

			return *this;
		}

		Vector3& operator+=(const Vector3& other)
		{
			X() += other.X();
			Y() += other.Y();
			Z() += other.Z();

			return *this;
		}

		Vector3 operator-(const float32 other) const
		{
			return { X() - other, Y() - other, Z() - other };
		}

		Vector3 operator-(const Vector3& other) const
		{
			return { X() - other.X(), Y() - other.Y(), Z() - other.Z() };
		}

		Vector3& operator-=(const float32 other)
		{
			X() -= other;
			Y() -= other;
			Z() -= other;

			return *this;
		}

		Vector3& operator-=(const Vector3& other)
		{
			X() -= other.X();
			Y() -= other.Y();
			Z() -= other.Z();

			return *this;
		}

		Vector3 operator*(const float32 other) const
		{
			return { X() * other, Y() * other, Z() * other };
		}

		Vector3 operator*(const Vector3& other) const
		{
			return { X() * other.X(), Y() * other.Y(), Z() * other.Z() };
		}

		Vector3& operator*=(const float32 other)
		{
			X() *= other;
			Y() *= other;
			Z() *= other;

			return *this;
		}

		Vector3& operator*=(const class Quaternion& quaternion);

		Vector3 operator/(const float32 other) const
		{
			return { X() / other, Y() / other, Z() / other };
		}

		Vector3& operator/=(const float32 other)
		{
			X() /= other;
			Y() /= other;
			Z() /= other;

			return *this;
		}

		bool operator==(const Vector3& other) const
		{
			return X() == other.X() && Y() == other.Y() && Z() == other.Z();
		}

		bool operator!=(const Vector3& other) const
		{
			return X() != other.X() || Y() != other.Y() || Z() != other.Z();
		}

		friend Vector3 operator*(const float32 lhs, const Vector3& rhs)
		{
			return Vector3(rhs.X() * lhs, rhs.Y() * lhs, rhs.Z() * lhs);
		}
		
		float32& X() { return values[0]; }
		float32& Y() { return values[1]; }
		float32& Z() { return values[2]; }

		float32 X() const { return values[0]; }
		float32 Y() const { return values[1]; }
		float32 Z() const { return values[2]; }

		float32* GetData() { return values; }
		const float32* GetData() const { return values; }

		float32 operator[](const uint8 i) const { return values[i]; }
		float32& operator[](const uint8 i) { return values[i]; }
	private:
		float32 values[4]{ 0.0f };
	};
}