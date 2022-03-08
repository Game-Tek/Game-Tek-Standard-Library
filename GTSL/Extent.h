#pragma once

#include "Core.h"

namespace GTSL {
	struct Extent2D {
		Extent2D() = default;

		Extent2D(struct Extent3D extent) noexcept;
		Extent2D(const uint16 width, const uint16 height) noexcept : Width(width), Height(height) {}

		uint16 Width = 0, Height = 0;

		bool operator==(const Extent2D& other) const { return Width == other.Width && Height == other.Height; }
		bool operator!=(const Extent2D& other) const { return Width != other.Width || Height != other.Height; }
		bool operator==(const uint16 other) const { return Width == other && Height == other; }
		bool operator!=(const uint16 other) const { return Width != other || Height != other; }

		Extent2D operator+(const Extent2D other) const { return { static_cast<uint16>(Width + other.Width), static_cast<uint16>(Height + other.Height) }; }
		Extent2D operator-(const Extent2D other) const { return { static_cast<uint16>(Width - other.Width), static_cast<uint16>(Height - other.Height) }; }
		Extent2D operator*(const Extent2D other) const { return { static_cast<uint16>(Width * other.Width), static_cast<uint16>(Height * other.Height) }; }
		Extent2D operator/(const Extent2D other) const { return { static_cast<uint16>(Width / other.Width), static_cast<uint16>(Height / other.Height) }; }
	};


	struct Extent3D {
		Extent3D() = default;

		Extent3D(const Extent2D extent) noexcept : Width(extent.Width), Height(extent.Height), Depth(1) {}
		Extent3D(const uint16 x) noexcept : Width(x), Height(x), Depth(x) {}
		Extent3D(const uint16 width, const uint16 height, const uint16 depth) noexcept : Width(width), Height(height), Depth(depth) {}
		
		uint16 Width = 0, Height = 0, Depth = 0;

		bool operator==(const Extent3D& other) const { return Width == other.Width && Height == other.Height && Depth == other.Depth; }
		bool operator!=(const Extent3D& other) const { return Width != other.Width || Height != other.Height || Depth != other.Depth; }
		bool operator==(const uint16 other) const { return Width == other && Height == other && Depth == other; }
		bool operator!=(const uint16 other) const { return Width != other || Height != other || Depth != other; }

		Extent3D operator+(const Extent3D other) const { return { static_cast<uint16>(Width + other.Width), static_cast<uint16>(Height + other.Height), static_cast<uint16>(Depth + other.Depth) }; }
		Extent3D operator-(const Extent3D other) const { return { static_cast<uint16>(Width - other.Width), static_cast<uint16>(Height - other.Height), static_cast<uint16>(Depth - other.Depth) }; }
		Extent3D operator*(const Extent3D other) const { return { static_cast<uint16>(Width * other.Width), static_cast<uint16>(Height * other.Height), static_cast<uint16>(Depth * other.Depth) }; }
		Extent3D operator/(const Extent3D other) const { return { static_cast<uint16>(Width / other.Width), static_cast<uint16>(Height / other.Height), static_cast<uint16>(Depth / other.Depth) }; }
	};
	
	inline Extent2D::Extent2D(const Extent3D extent) noexcept: Width(extent.Width), Height(extent.Height) {}
}