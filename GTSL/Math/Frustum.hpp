#pragma once

#include "Plane.hpp"

namespace GTSL
{
	class Frustum
	{
	public:
		Frustum() = default;

		[[nodiscard]] Plane& GetTopPlane() { return Planes[0]; }
		[[nodiscard]] Plane& GetRightPlane() { return Planes[1]; }
		[[nodiscard]] Plane& GetBottomPlane() { return Planes[2]; }
		[[nodiscard]] Plane& GetLeftPlane() { return Planes[3]; }
		[[nodiscard]] Plane& GetFrontPlane() { return Planes[4]; }
		[[nodiscard]] Plane& GetBackPlane() { return Planes[5]; }

		[[nodiscard]] Plane* GetPlanes() { return Planes; }
	private:
		Plane m_planes[6];
	};
}