#include "GTSL/Math/Vector4.h"

#include "GTSL/Math/Vector3.h"

GTSL::Vector4::Vector4(const Vector3& vector3) : Vector4(vector3.X(), vector3.Y(), vector3.Z(), 0.0f)
{
}
