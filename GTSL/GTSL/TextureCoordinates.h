#pragma once

#include "Core.h"

namespace GTSL
{	
	struct TextureCoordinates2D
	{
		TextureCoordinates2D(const float32 u, const float32 v) noexcept : array{ u, v } {}

		float32& U() { return array[0]; }
		float32& V() { return array[1]; }
		[[nodiscard]] float32 U() const { return array[0]; }
		[[nodiscard]] float32 V() const { return array[1]; }

		float32& operator[](const uint8 i) { return array[i]; }
		[[nodiscard]] float32 operator[](const uint8 i) const { return array[i]; }
	private:
		float32 array[2]{ 0.0f };
	};

	struct TextureCoordinate3D
	{
		TextureCoordinate3D(const float32 u, const float32 v, const float32 w) noexcept : array{ u, v, w } {}
		
		float32& U() { return array[0]; }
		float32& V() { return array[1]; }
		float32& W() { return array[2]; }
		[[nodiscard]] float32 U() const { return array[0]; }
		[[nodiscard]] float32 V() const { return array[1]; }
		[[nodiscard]] float32 W() const { return array[2]; }

		float32& operator[](const uint8 i) { return array[i]; }
		[[nodiscard]] float32 operator[](const uint8 i) const { return array[i]; }
	private:
		float32 array[3]{ 0.0f };
	};
}
