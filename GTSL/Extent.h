#pragma once

#include "Core.h"

namespace GTSL
{
	struct Extent2D
	{
		Extent2D() = default;

		Extent2D(struct Extent3D extent) noexcept;
		Extent2D(const uint16 width, const uint16 height) noexcept : Width(width), Height(height) {}

		uint16 Width = 0, Height = 0;

		bool operator==(const Extent2D& other) const { return Width == other.Width && Height == other.Height; }
		bool operator!=(const Extent2D& other) const { return Width != other.Width && Height != other.Height; }
		bool operator==(const uint16 other) const { return Width == other && Height == other; }
		bool operator!=(const uint16 other) const { return Width != other && Height != other; }
	};


	struct Extent3D
	{
		Extent3D() = default;

		Extent3D(const Extent2D extent) noexcept : Width(extent.Width), Height(extent.Height), Depth(1) {}
		Extent3D(const uint16 width, const uint16 height, const uint16 depth) noexcept : Width(width), Height(height), Depth(depth) {}
		
		uint16 Width = 0, Height = 0, Depth = 0;
	};
	
	inline Extent2D::Extent2D(const Extent3D extent) noexcept: Width(extent.Width), Height(extent.Height)
	{
	}
}