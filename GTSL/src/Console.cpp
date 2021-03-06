#include "GTSL/Console.h"

#if (_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

void GTSL::Console::Print(const Range<const UTF8*> text)
{
	DWORD chars_written{ 0 };
	WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), text.begin(), text.Bytes(), &chars_written, nullptr);
}

void GTSL::Console::Read(Range<UTF8*> buffer)
{
	DWORD characters_read{ 0 };
	ReadConsoleA(GetStdHandle(STD_INPUT_HANDLE), buffer.begin(), buffer.ElementCount(), &characters_read, nullptr);
	buffer = Range<UTF8*>(characters_read, buffer.begin());
}

void GTSL::Console::SetTextColor(const ConsoleTextColor textColor)
{
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
