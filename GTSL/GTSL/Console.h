#pragma once

#include "Core.h"
#include "Ranger.h"

namespace GTSL
{
	class Console
	{
	public:
		static void Print(const Ranger<const UTF8>& text);
		static void Read(Ranger<UTF8>& buffer);

		enum class ConsoleTextColor : uint8
		{
			WHITE, RED, YELLOW, GREEN, ORANGE, PURPLE
		};
		static void SetTextColor(ConsoleTextColor textColor);
	};
}
