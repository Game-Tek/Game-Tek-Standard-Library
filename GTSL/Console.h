#pragma once

#include "Core.h"
#include "Range.hpp"

#if (_WIN64)
#include <Windows.h>
#endif

namespace GTSL
{
	class Console
	{
	public:
		static void SetConsoleInputModeAsUTF8() {
			SetConsoleOutputCP(CP_UTF8);
		}

		static void Print(const Range<const char8_t*> text) {
			DWORD chars_written{ 0 };
			WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), text.begin(), text.Bytes(), &chars_written, nullptr);
		}
		
		static void Read(Range<char8_t*> buffer) {
			DWORD characters_read{ 0 };
			ReadConsoleA(GetStdHandle(STD_INPUT_HANDLE), buffer.begin(), buffer.ElementCount(), &characters_read, nullptr);
			buffer = Range<char8_t*>(characters_read, buffer.begin());
		}

		enum class ConsoleTextColor : uint8
		{
			WHITE, RED, YELLOW, GREEN, ORANGE, PURPLE
		};
		static void SetTextColor(ConsoleTextColor textColor) {
			WORD color{ 0 };

			switch (textColor)
			{
			case ConsoleTextColor::WHITE:	color = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; break;
			case ConsoleTextColor::RED:		color = FOREGROUND_INTENSITY | FOREGROUND_RED; break;
			case ConsoleTextColor::YELLOW:	color = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN; break;
			case ConsoleTextColor::GREEN:	color = FOREGROUND_INTENSITY | FOREGROUND_GREEN; break;
			case ConsoleTextColor::ORANGE:	color = FOREGROUND_RED | FOREGROUND_GREEN; break;
			case ConsoleTextColor::PURPLE:	color = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE; break;
			default:						break;
			}

			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
		}
	};
}
