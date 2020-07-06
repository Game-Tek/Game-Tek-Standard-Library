#pragma once

#include "Time.h"

namespace GTSL
{
	using Byte = RatioUnit<uint64, 1, 1>;
	using KiloByte = RatioUnit<uint64, 1024, 1>;
	using MegaByte = RatioUnit<uint64, 1024 * 1024, 1>;
	using GigaByte = RatioUnit<uint64, 1024 * 1024 * 1024, 1>;
}
