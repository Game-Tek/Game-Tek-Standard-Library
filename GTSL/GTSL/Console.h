#pragma once

#include "Core.h"
#include "Ranger.h"

namespace GTSL
{
	class Console
	{
	public:
		Console() = default;
		
		Console(const Ranger<const UTF8>& text);

		~Console();
		
		void Print(const Ranger<const UTF8>& text) const;
		void Read(Ranger<UTF8>& buffer) const;

		enum class ConsoleTextColor : uint8
		{
			WHITE, RED, YELLOW, GREEN, ORANGE, PURPLE
		};
		void SetTextColor(ConsoleTextColor textColor) const;
	private:
		void* handle{ nullptr };
	};
}
