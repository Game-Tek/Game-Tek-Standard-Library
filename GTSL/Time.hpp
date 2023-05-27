#pragma once

#include "Core.hpp"
#include "RatioUnit.hpp"

namespace GTSL
{
	typedef RatioUnit<uint64, 1, 1000000000> Nanoseconds;
	typedef RatioUnit<uint64, 1, 1000000> Microseconds;
	typedef RatioUnit<uint64, 1, 1000> Milliseconds;
	typedef RatioUnit<uint64, 1, 1> Seconds;
	typedef RatioUnit<uint64, 60, 1> Minutes;
	typedef RatioUnit<uint64, 3600, 1> Hour;
	typedef RatioUnit<uint64, 86400, 1> Day;
}