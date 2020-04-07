#pragma once

#include "Array.hpp"

namespace GTSL
{
	template<uint32 N, typename CL = char>
	class StaticString
	{
		Array<CL, N> array;
	public:
		constexpr StaticString() noexcept = default;
	};
}
