#pragma once

namespace GTSL
{
	//Used to specify a location in 2D space with floating point precision.
	class Vector2
	{
	public:
		Vector2() = default;

		constexpr Vector2(float32 a) : values{ a, a }
		{
		}

		constexpr Vector2(float32 x, float32 y) : values{ x, y }
		{
		}

		~Vector2() = default;

		Vector2 operator+(float Other) const
		{
			return { X() + Other, Y() + Other };
		}

		Vector2 operator+(const Vector2& Other) const
		{
			return { X() + Other.X(), Y() + Other.Y() };
		}

		Vector2& operator+=(float Other)
		{
			X() += Other;
			Y() += Other;

			return *this;
		}

		Vector2& operator+=(const Vector2& Other)
		{
			X() += Other.X();
			Y() += Other.Y();

			return *this;
		}

		Vector2 operator-(float Other) const
		{
			return { X() - Other, Y() - Other };
		}

		Vector2 operator-(const Vector2& Other) const
		{
			return { X() - Other.X(), Y() - Other.Y() };
		}

		Vector2& operator-=(float Other)
		{
			X() -= Other;
			Y() -= Other;

			return *this;
		}

		Vector2& operator-=(const Vector2& Other)
		{
			X() -= Other.X();
			Y() -= Other.Y();

			return *this;
		}

		Vector2 operator*(float Other) const
		{
			return { X() * Other, Y() * Other };
		}

		Vector2& operator*=(float Other)
		{
			X() *= Other;
			Y() *= Other;

			return *this;
		}

		Vector2 operator/(float Other) const
		{
			return { X() / Other, Y() / Other };
		}

		Vector2& operator/=(float Other)
		{
			X() /= Other;
			Y() /= Other;

			return *this;
		}

		inline bool operator==(const Vector2& Other)
		{
			return X() == Other.X() && Y() == Other.Y();
		}

		inline bool operator!=(const Vector2& Other)
		{
			return X() != Other.X() || Y() != Other.Y();
		}

		float32& X() { return values[0]; }
		float32& Y() { return values[1]; }

		float32 X() const { return values[0]; }
		float32 Y() const { return values[1]; }

		float32* GetData() { return values; }
		const float32* GetData() const { return values; }

		float32 operator[](const uint8 i) const { return values[i]; }
		float32& operator[](const uint8 i) { return values[i]; }
	
	private:
		float32 values[2]{ 0.0f };
	};
}