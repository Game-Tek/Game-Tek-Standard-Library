#pragma once

#include "Core.hpp"
#include "RatioUnit.hpp"

typedef GTSL::RatioUnit<GTSL::uint64, 1, 1> Byte;
typedef GTSL::RatioUnit<GTSL::uint64, 1024, 1> KiloByte;
typedef GTSL::RatioUnit<GTSL::uint64, 1024 * 1024, 1> MegaByte;
typedef GTSL::RatioUnit<GTSL::uint64, 1024u * 1024u * 1024u, 1> GigaByte;