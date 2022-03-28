#pragma once

#include "GTSL/Core.h"

namespace GTSL
{
	class Vector2;
	class Vector3;
	class Vector4;

	//Used to specify a location in 2D space with floating point precision.
	class Vector2
	{
	public:
		Vector2() = default;

		constexpr Vector2(float32 a) : values{ a, a } {}
		constexpr Vector2(float32 x, float32 y) : values{ x, y } {}
		explicit Vector2(Vector3 other);

		~Vector2() = default;

		Vector2 operator+(float32 other) const { return { X() + other, Y() + other }; }
		Vector2 operator+(const Vector2 other) const { return { X() + other.X(), Y() + other.Y() }; }

		Vector2& operator+=(float other) { X() += other; Y() += other; return *this; }
		Vector2& operator+=(const Vector2 other) { X() += other.X(); Y() += other.Y(); return *this; }

		Vector2 operator-(float other) const { return { X() - other, Y() - other }; }
		Vector2 operator-(const Vector2 other) const { return { X() - other.X(), Y() - other.Y() }; }

		Vector2& operator-=(float other) { X() -= other; Y() -= other; return *this; }
		Vector2& operator-=(const Vector2 other) { X() -= other.X(); Y() -= other.Y(); return *this; }

		Vector2 operator*(float other) const { return { X() * other, Y() * other }; }
		Vector2 operator*(Vector2 other) const { return { X() * other.X(), Y() * other.Y() }; }

		Vector2& operator*=(float other) { X() *= other; Y() *= other; return *this; }
		Vector2& operator*=(Vector2 other) { X() *= other.X(); Y() *= other.Y(); return *this; }

		Vector2 operator/(float other) const { return { X() / other, Y() / other }; }
		Vector2 operator/(Vector2 other) const { return { X() / other.X(), Y() / other.Y() }; }

		Vector2& operator/=(float other) { X() /= other; Y() /= other; return *this; }
		Vector2& operator/=(Vector2 other) { X() /= other.X(); Y() /= other.Y(); return *this; }
		
		bool operator==(const Vector2 other) const { return X() == other.X() && Y() == other.Y(); }
		
		bool operator!=(const Vector2 other) const { return X() != other.X() || Y() != other.Y(); }

		bool operator>(const Vector2 other) const { return X() > other.X() and Y() > other.Y(); }
		bool operator>=(const Vector2 other) const { return X() >= other.X() and Y() >= other.Y(); }

		bool operator<(const Vector2 other) const { return X() < other.X() and Y() < other.Y(); }
		bool operator<=(const Vector2 other) const { return X() <= other.X() and Y() <= other.Y(); }

		float32& X() { return values[0]; }
		float32& Y() { return values[1]; }

		constexpr float32 X() const { return values[0]; }
		constexpr float32 Y() const { return values[1]; }

		float32* GetData() { return values; }
		const float32* GetData() const { return values; }

		float32 operator[](const uint8 i) const { return values[i]; }
		float32& operator[](const uint8 i) { return values[i]; }

	private:
		float32 values[2]{ 0.0f };
	};

	//Used to specify a location in 3D space with floating point precision.
	class Vector3 {
	public:
		Vector3() = default;

		constexpr Vector3(const float32 a) : values{ a, a, a } {}

		constexpr Vector3(const Vector2 a, const float32 z) : values{ a.X(), a.Y(), z} {}
		constexpr Vector3(const float32 x, const float32 y, const float32 z) : values{ x, y, z } {}

		explicit Vector3(const class Rotator& rotator);

		explicit Vector3(const Vector4 vector4);

		Vector3(const Vector3& other) = default;

		~Vector3() = default;

		Vector3 operator+(const float32 other) const { return { X() + other, Y() + other, Z() + other }; }
		Vector3 operator+(const Vector3& other) const { return { X() + other.X(), Y() + other.Y(), Z() + other.Z() }; }

		Vector3& operator+=(const float32 other) { X() += other; Y() += other; Z() += other; return *this; }
		Vector3& operator+=(const Vector3& other) { X() += other.X(); Y() += other.Y(); Z() += other.Z(); return *this; }

		Vector3 operator-() const { return { -X(), -Y(), -Z() }; }
		Vector3 operator-(const float32 other) const { return { X() - other, Y() - other, Z() - other }; }
		Vector3 operator-(const Vector3& other) const { return { X() - other.X(), Y() - other.Y(), Z() - other.Z() }; }

		Vector3& operator-=(const float32 other) { X() -= other; Y() -= other; Z() -= other; return *this; }
		Vector3& operator-=(const Vector3& other) { X() -= other.X(); Y() -= other.Y(); Z() -= other.Z(); return *this; }

		Vector3 operator*(const float32 other) const { return { X() * other, Y() * other, Z() * other }; }
		Vector3 operator*(const Vector3& other) const { return { X() * other.X(), Y() * other.Y(), Z() * other.Z() }; }

		Vector3& operator*=(const float32 other) { X() *= other; Y() *= other; Z() *= other; return *this; }
		Vector3& operator*=(const class Quaternion& quaternion);

		Vector3 operator/(const float32 other) const { return { X() / other, Y() / other, Z() / other }; }
		Vector3& operator/=(const float32 other) { X() /= other; Y() /= other; Z() /= other; return *this; }

		bool operator==(const Vector3& other) const { return X() == other.X() && Y() == other.Y() && Z() == other.Z(); }
		bool operator!=(const Vector3& other) const { return X() != other.X() || Y() != other.Y() || Z() != other.Z(); }

		bool operator>(const Vector3& other) const { return X() > other.X() and Y() > other.Y() and Z() > other.Z(); }
		bool operator>=(const Vector3& other) const { return X() >= other.X() and Y() >= other.Y() and Z() >= other.Z(); }
		bool operator<(const Vector3& other) const { return X() < other.X() and Y() < other.Y() and Z() < other.Z(); }
		bool operator<=(const Vector3& other) const { return X() <= other.X() and Y() <= other.Y() and Z() <= other.Z(); }

		friend Vector3 operator*(const float32 lhs, const Vector3& rhs) { return { rhs.X() * lhs, rhs.Y() * lhs, rhs.Z() * lhs }; }

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
		float32 values[3]{ 0.0f };
	};
	
	class alignas(16) Vector4 {
	public:
		Vector4() = default;

		Vector4(const float32 a) : values{ a, a, a, a } {}
		Vector4(const float32 x, const float32 y, const float32 z, const float32 w) : values{ x, y, z, w } {}

		Vector4(const Vector3 other, const float32 w) : Vector4(other.X(), other.Y(), other.Z(), w) {}
		Vector4(const Vector3 other) : Vector4(other, 1.0f) {}
		
		~Vector4() = default;

		Vector4 operator+(const float other) const { return { X() + other, Y() + other, Z() + other, W() + other }; }
		Vector4 operator+(const Vector4& other) const { return { X() + other.X(), Y() + other.Y(), Z() + other.Z(), W() + other.W() }; }

		Vector4& operator+=(float Other) { X() += Other; Y() += Other; Z() += Other; W() += Other; return *this; }
		Vector4& operator+=(const Vector4& Other) { X() += Other.X(); Y() += Other.Y(); Z() += Other.Z(); W() += Other.W(); return *this; }

		Vector4 operator-(float Other) const { return { X() - Other, Y() - Other, Z() - Other, W() - Other }; }
		Vector4 operator-(const Vector4& Other) const { return { X() - Other.X(), Y() - Other.Y(), Z() - Other.Z(), W() - Other.W() }; }

		Vector4& operator-=(float Other) { X() -= Other; Y() -= Other; Z() -= Other; W() -= Other; return *this; }
		Vector4& operator-=(const Vector4& Other) { X() -= Other.X(); Y() -= Other.Y(); Z() -= Other.Z(); W() -= Other.W(); return *this; }

		Vector4 operator*(float32 other) const { return { X() * other, Y() * other, Z() * other, W() * other }; }
		Vector4& operator*=(float32 other) { X() *= other; Y() *= other; Z() *= other; W() *= other; return *this; }

		Vector4 operator/(float32 other) const { const auto div = 1.f / other; return { X() * div, Y() * div, Z() * div, W() * div }; }
		Vector4& operator/=(float32 other) { X() /= other; Y() /= other; Z() /= other; W() /= other; return *this; }

		bool operator==(const Vector4& Other) const { return X() == Other.X() && Y() == Other.Y() && Z() == Other.Z() && W() == Other.W(); }
		bool operator!=(const Vector4& Other) const { return X() != Other.X() || Y() != Other.Y() || Z() != Other.Z() || W() != Other.W(); }

		bool operator>(const Vector4& other) const { return X() > other.X() and Y() > other.Y() and Z() > other.Z() and W() > other.W(); }
		bool operator>=(const Vector4& other) const { return X() >= other.X() and Y() >= other.Y() and Z() >= other.Z() and W() >= other.W(); }

		bool operator<(const Vector4& other) const { return X() < other.X() and Y() < other.Y() and Z() < other.Z() and W() < other.W(); }
		bool operator<=(const Vector4& other) const { return X() <= other.X() and Y() <= other.Y() and Z() <= other.Z() and W() <= other.W(); }

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

	inline Vector2::Vector2(const Vector3 other) : values{ other.X(), other.Y() } {}

	inline Vector3::Vector3(const Vector4 vector4) : values{ vector4.X(), vector4.Y(), vector4.Z() } {}
}