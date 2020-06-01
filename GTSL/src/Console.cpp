#include "GTSL/Console.h"

#if (_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

GTSL::Console::Console() : handle(GetStdHandle(STD_OUTPUT_HANDLE))
{
}

GTSL::Console::Console(const Ranger<const UTF8>& text)
{
	AllocConsole();
	handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTitleA(text.begin());
}

GTSL::Console::~Console()
{
	FreeConsole();
}

void GTSL::Console::Print(const Ranger<const UTF8>& text) const
{
	DWORD chars_written{ 0 };
	WriteConsoleA(handle, text.begin(), text.Bytes(), &chars_written, nullptr);
}

void GTSL::Console::Read(Ranger<UTF8>& buffer) const
{
	DWORD characters_read{ 0 };
	ReadConsoleA(GetStdHandle(STD_INPUT_HANDLE), buffer.begin(), buffer.ElementCount(), &characters_read, nullptr);
	buffer = Ranger<UTF8>(characters_read, buffer.begin());
}

void GTSL::Console::SetTextColor(const ConsoleTextColor textColor) const
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

	SetConsoleTextAttribute(handle, color);
}
