#include "Math/Rotator.h"

#include "Math/Math.hpp"

//there seems to be some inaccuracy in the X field, CHECK, but works fairly well for now

using namespace GTSL;

Rotator::Rotator(const Vector3& vector) : X(Math::ArcSine(vector.Y)), Y(Math::ArcSine(vector.X / Math::Cosine(X))), Z(0)
{
}
