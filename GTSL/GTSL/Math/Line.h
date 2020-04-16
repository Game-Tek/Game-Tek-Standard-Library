#pragma once

#include "Core.h"

#include "Vector3.h"
#include "Math.hpp"

namespace GTSL
{
	class Line3
	{
	public:
		Vector3 Start;
		Vector3 End;

		[[nodiscard]] float Length() const
		{
			return GTSL::Math::Length(Segment());
		}

		[[nodiscard]] float LengthSquared() const
		{
			return Math::LengthSquared(Segment());
		}

	private:
		[[nodiscard]] Vector3 Segment() const
		{
			return End - Start;
		}
	};
}
