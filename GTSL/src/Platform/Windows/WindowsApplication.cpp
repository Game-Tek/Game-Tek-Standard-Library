#include "Platform/Windows/WindowsApplication.h"

GTSL::WindowsApplication::WindowsApplication(const ApplicationCreateInfo& applicationCreateInfo) : Application(applicationCreateInfo), instance(GetModuleHandle(nullptr))
{
}

void GTSL::WindowsApplication::Update()
{
	MSG message;
	
	while (GetMessageA(&message, nullptr, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessageA(&message);
	}
}

void GTSL::WindowsApplication::Close()
{
	PostQuitMessage(0);
}

void GTSL::WindowsApplication::GetNativeHandles(void* nativeHandles)
{
	static_cast<Win32NativeHandles*>(nativeHandles)->HINSTANCE = instance;
}
