#include "GTSL/Math/Plane.h"

#include "GTSL/Math/Math.hpp"

using namespace GTSL;

Plane::Plane(const Vector3& a, const Vector3& b, const Vector3& c) : Normal(Math::Normalized(Math::Cross(b - a, c - a))), D(Math::DotProduct(Normal, a))
{
}
