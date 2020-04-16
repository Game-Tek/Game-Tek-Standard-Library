#include "GTSL/Platform/Windows/WindowsApplication.h"

#include "GTSL/Application.h"

using namespace GTSL;

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

void WindowsApplication::GetNativeHandles(void* nativeHandles)
{
	static_cast<Win32NativeHandles*>(nativeHandles)->HINSTANCE = instance;
}
