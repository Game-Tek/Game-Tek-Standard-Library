#include "GTSL/Math/Vector4.h"

#include "GTSL/Math/Vector3.h"

GTSL::Vector4::Vector4(const Vector3& vector3) : X(vector3.X), Y(vector3.Y), Z(vector3.Z), W(1.0f)
{
}
