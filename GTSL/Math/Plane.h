#pragma once

#include "Vectors.h"

namespace GTSL
{
	struct Plane
	{
		Plane() = default;
		Plane(const Vector3& a, const Vector3& b, const Vector3& c);

		Vector3 Normal;
		float32 D = 0.0f;
	};
}
