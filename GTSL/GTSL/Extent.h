#pragma once

#include "Core.h"

namespace GTSL
{
	struct Extent2D
	{
		Extent2D() = default;
		
		Extent2D(const uint16 width, const uint16 height) noexcept : Width(width), Height(height) {}

		uint16 Width = 0;
		uint16 Height = 0;
	};

	struct Extent3D
	{
		Extent3D() = default;

		Extent3D(const uint16 width, const uint16 height, const uint16 depth) noexcept : Width(width), Height(height), Depth(depth) {}
		
		uint16 Width = 0;
		uint16 Height = 0;
		uint16 Depth = 0;
	};
}