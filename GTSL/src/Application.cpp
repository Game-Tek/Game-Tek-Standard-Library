#include "GTSL/Application.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "GTSL/Window.h"

using namespace GTSL;

GTSL::Application::Application(const ApplicationCreateInfo& applicationCreateInfo)
{
}

GTSL::Application::~Application() = default;

void GTSL::Application::Update()
{

}

void GTSL::Application::UpdateWindow(Window* window)
{
	Window::Win32NativeHandles win32_native_handles;
	window->GetNativeHandles(&win32_native_handles);

	MSG message;

	while (GetMessageA(&message, reinterpret_cast<HWND>(win32_native_handles.HWND), 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessageA(&message);
	}
}

void GTSL::Application::Close()
{
	PostQuitMessage(0);
}

void GTSL::Application::GetNativeHandles(void* nativeHandles)
{
	static_cast<Win32NativeHandles*>(nativeHandles)->HINSTANCE = GetModuleHandle(nullptr);
}
