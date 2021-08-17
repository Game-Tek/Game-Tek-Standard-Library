#pragma once

#include "Core.h"
#include "RatioUnit.hpp"

namespace GTSL
{
	using SignedNanoseconds  = RatioUnit<int64, 1, 1000000000>;
	using SignedMicroseconds = RatioUnit<int64, 1, 1000000>;
	using SignedMilliseconds = RatioUnit<int64, 1, 1000>;
	using SignedSeconds      = RatioUnit<int64, 1, 1>;

	using Nanoseconds  = RatioUnit<uint64, 1, 1000000000>;
	using Microseconds = RatioUnit<uint64, 1, 1000000>;
	using Milliseconds = RatioUnit<uint64, 1, 1000>;
	using Seconds      = RatioUnit<uint64, 1, 1>;
	using Minutes      = RatioUnit<uint64, 60, 1>;
	using Hour		   = RatioUnit<uint64, 3600, 1>;
	using Day		   = RatioUnit<uint64, 86400, 1>;
	
	//template<typename T, uint64 ratio>
	//T TimeUnitCast(const UnsignedTimeUnit<ratio>& timeUnit) { return T(timeUnit.count / ratio); }
	//
	//template<typename T, uint64 ratio>
	//T TimeUnitCast(const SignedTimeUnit<ratio>& timeUnit) { return T(timeUnit.count / ratio); }
}
