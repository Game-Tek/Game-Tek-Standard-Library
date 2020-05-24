#include "GTSL/Application.h"

#if (_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include "GTSL/StaticString.hpp"
#include "GTSL/Window.h"

using namespace GTSL;

Application::Application(const ApplicationCreateInfo& applicationCreateInfo) : handle(GetModuleHandleA(NULL))
{

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

	while (PeekMessageA(&message, static_cast<HWND>(win32_native_handles.HWND), 0, 0, PM_REMOVE))
	{
		TranslateMessage(&message);
		DispatchMessageA(&message);
	}
}

void Application::SetProcessPriority(const Priority priority) noexcept
{
	int32 priority_class{ NORMAL_PRIORITY_CLASS };
	switch (priority)
	{
	case Priority::LOW: priority_class = IDLE_PRIORITY_CLASS; break;
	case Priority::LOW_MID: priority_class = BELOW_NORMAL_PRIORITY_CLASS; break;
	case Priority::MID: priority_class = NORMAL_PRIORITY_CLASS; break;
	case Priority::MID_HIGH: priority_class = ABOVE_NORMAL_PRIORITY_CLASS; break;
	case Priority::HIGH: priority_class = HIGH_PRIORITY_CLASS; break;
	default: break;
	}

	SetPriorityClass(GetCurrentProcess(), priority_class);
}

void Application::Close()
{
	PostQuitMessage(0);
}

void Application::GetNativeHandles(void* nativeHandles)
{
	static_cast<Win32NativeHandles*>(nativeHandles)->HINSTANCE = static_cast<HMODULE>(handle);
}

uint8 Application::ThreadCount() noexcept {	SYSTEM_INFO system_info; GetSystemInfo(&system_info); return system_info.dwNumberOfProcessors; }

StaticString<Application::MaxPathLength> Application::GetPathToExecutable() const
{
	StaticString<Application::MaxPathLength> ret;
	ret.Resize(GetModuleFileNameA(static_cast<HMODULE>(handle), ret.begin(), ret.GetCapacity()) + 1);
	//GTSL_ASSERT(GetModuleFileNameA(NULL, a, 512), "Failed to get Win32 module file name!")
	ret.ReplaceAll('\\', '/');
	return ret;
}
