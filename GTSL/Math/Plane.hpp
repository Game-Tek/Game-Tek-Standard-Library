#pragma once

#include "Vectors.hpp"

namespace GTSL
{
	struct Plane
	{
	public:
		Plane() = default;
		Plane(const Vector3& a, const Vector3& b, const Vector3& c);

		Vector3 Normal;
		float D = 0;
	};
}