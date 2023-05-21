#pragma once

#include "Vectors.hpp"

namespace GTSL
{
	using float32 = float;
	
	class Line3
	{
	public:
		Vector3 Start;
		Vector3 End;

		//[[nodiscard]] float32 Length() const
		//{
		//	return GTSL::Math::Length(Segment());
		//}
		//
		//[[nodiscard]] float32 LengthSquared() const
		//{
		//	return Math::LengthSquared(Segment());
		//}

	private:
		[[nodiscard]] Vector3 Segment() const
		{
			return End - Start;
		}
	};
}
