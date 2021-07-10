#pragma once

#include "Core.h"
#include "Range.h"

namespace GTSL
{
	class Console
	{
	public:
		static void Print(const Range<const char8_t*> text);
		static void Read(Range<char8_t*> buffer);

		enum class ConsoleTextColor : uint8
		{
			WHITE, RED, YELLOW, GREEN, ORANGE, PURPLE
		};
		static void SetTextColor(ConsoleTextColor textColor);
	};
}
