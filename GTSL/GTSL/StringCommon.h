#pragma once
#include <GTSL/Core.h>

#include "Ranger.h"

namespace GTSL
{
	void ToString(int8 num, Ranger<UTF8>& buffer);
	void ToString(uint8 num, Ranger<UTF8>& buffer);
	void ToString(int16 num, Ranger<UTF8>& buffer);
	void ToString(uint16 num, Ranger<UTF8>& buffer);
	void ToString(int32 num, Ranger<UTF8>& buffer);
	void ToString(uint32 num, Ranger<UTF8>& buffer);
	void ToString(int64 num, Ranger<UTF8>& buffer);
	void ToString(uint64 num, Ranger<UTF8>& buffer);
	void ToString(float32 num, Ranger<UTF8>& buffer);
	void ToString(float64 num, Ranger<UTF8>& buffer);

	constexpr uint32 stringLength(const char* text) noexcept
	{
		uint32 i{ 0 }; while (text[i] != '\0') { ++i; } return i + 1;
	}
}
