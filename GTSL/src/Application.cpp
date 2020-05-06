#include "GTSL/Application.h"

#if (_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include "GTSL/StaticString.hpp"
#include "GTSL/Window.h"

using namespace GTSL;

Application::Application(const ApplicationCreateInfo& applicationCreateInfo)
{
	path.Resize(GetModuleFileNameA(NULL, path.begin(), path.GetCapacity()) + 1);
	//GTSL_ASSERT(GetModuleFileNameA(NULL, a, 512), "Failed to get Win32 module file name!")
	path.Drop(path.FindLast('\\') + 1);
	path.ReplaceAll('\\', '/');
}

Application::~Application() = default;

void Application::Update()
{

}

void Application::UpdateWindow(Window* window)
{
	Window::Win32NativeHandles win32_native_handles;
	window->GetNativeHandles(&win32_native_handles);

	MSG message;

	while (GetMessageA(&message, static_cast<HWND>(win32_native_handles.HWND), 0, 0) > 0)
	{
		TranslateMessage(&message); DispatchMessageA(&message);
	}
}

void Application::Close()
{
	PostQuitMessage(0);
}

void Application::GetNativeHandles(void* nativeHandles)
{
	static_cast<Win32NativeHandles*>(nativeHandles)->HINSTANCE = GetModuleHandle(nullptr);
}

Ranger<char> Application::GetPathToExecutable()
{
	return path;
}
