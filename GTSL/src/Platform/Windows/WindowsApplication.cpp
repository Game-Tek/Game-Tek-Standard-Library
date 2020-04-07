#include "Platform/Windows/WindowsApplication.h"

WindowsApplication::WindowsApplication(const ApplicationCreateInfo& applicationCreateInfo) : Application(applicationCreateInfo), instance(GetModuleHandle(nullptr))
{
}

void WindowsApplication::Update()
{
	MSG message;
	
	while (GetMessageA(&message, nullptr, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessageA(&message);
	}
}

void WindowsApplication::Close()
{
	PostQuitMessage(0);
}