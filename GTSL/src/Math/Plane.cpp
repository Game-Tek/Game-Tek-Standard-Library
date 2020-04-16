#include "GTSL/Math/Plane.h"

#include "GTSL/Math/Vector3.h"
#include "GTSL/Math/Math.hpp"

using namespace GTSL;

Plane::Plane(const Vector3& _A, const Vector3& _B, const Vector3& _C) : Normal(Math::Normalized(Math::Cross(_B - _A, _C - _A))), D(Math::DotProduct(Normal, _A))
{
}
