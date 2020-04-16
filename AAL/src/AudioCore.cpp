#include "AAL/AudioCore.h"

#include <GTSL/Math/Math.hpp>

float dBToVolume(const float db) { return GTSL::Math::Power(10.0f, 0.05f * db); }

float VolumeTodB(const float volume) { return 20.0f * GTSL::Math::Log10(volume); }
