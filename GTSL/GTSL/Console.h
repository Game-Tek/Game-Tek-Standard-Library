#pragma once

#include "Core.h"
#include "Range.h"

namespace GTSL
{
	class Console
	{
	public:
		static void Print(const Range<const UTF8*> text);
		static void Read(Range<UTF8*> buffer);

		enum class ConsoleTextColor : uint8
		{
			WHITE, RED, YELLOW, GREEN, ORANGE, PURPLE
		};
		static void SetTextColor(ConsoleTextColor textColor);
	};
}
