#include "GTSL/Window.h"

#include <Windows.h>
#include "GTSL/Application.h"

#if (_WIN32)
GTSL::uint64 GTSL::Window::Win32_windowProc(void* hwnd, uint32 uMsg, uint64 wParam, uint64 lParam)
{
	const auto window = reinterpret_cast<Window*>(GetWindowLongPtrA(reinterpret_cast<HWND>(hwnd), GWLP_USERDATA));
	auto win_handle = reinterpret_cast<HWND>(hwnd);
	
	switch (uMsg)
	{
	case WM_CLOSE:
	{
		window->onCloseDelegate();	return 0;
	}
	case WM_MOUSEMOVE: /*absolute pos*/
	{
		const auto mouse_pos = window->mousePos;
		window->Win32_calculateMousePos(LOWORD(lParam), HIWORD(lParam));
		window->onMouseMove(window->mousePos, window->mousePos - mouse_pos); return 0;
	}
	case WM_MOUSEHWHEEL:
	{
		window->onMouseWheelMove(GET_WHEEL_DELTA_WPARAM(wParam)); return 0;
	}
	case WM_LBUTTONDOWN:
	{
		window->onMouseButtonClick(MouseButton::LEFT_BUTTON, ButtonState::PRESSED);  return 0;
	}
	case WM_LBUTTONUP:
	{
		window->onMouseButtonClick(MouseButton::LEFT_BUTTON, ButtonState::RELEASED); return 0;
	}
	case WM_RBUTTONDOWN:
	{
		window->onMouseButtonClick(MouseButton::RIGHT_BUTTON, ButtonState::PRESSED);  return 0;
	}
	case WM_RBUTTONUP:
	{
		window->onMouseButtonClick(MouseButton::RIGHT_BUTTON, ButtonState::RELEASED); return 0;
	}
	case WM_MBUTTONDOWN:
	{
		window->onMouseButtonClick(MouseButton::MIDDLE_BUTTON, ButtonState::PRESSED);  return 0;
	}
	case WM_MBUTTONUP:
	{
		window->onMouseButtonClick(MouseButton::MIDDLE_BUTTON, ButtonState::RELEASED); return 0;
	}
	case WM_KEYDOWN:
	{
		KeyboardKeys keyboard_keys;
		Win32_translateKeys(wParam, lParam, keyboard_keys);
		window->onKeyEvent(keyboard_keys, ButtonState::PRESSED);  return 0;
	}
	case WM_KEYUP:
	{
		KeyboardKeys keyboard_keys;
		Win32_translateKeys(wParam, lParam, keyboard_keys);
		window->onKeyEvent(keyboard_keys, ButtonState::RELEASED); return 0;
	}
	case WM_CHAR:
	{
		window->onCharEvent(wParam);
	}
	case WM_SIZE:
	{
		RECT rect;
		GetWindowRect(win_handle, &rect);
		window->windowSize.Width = rect.right - rect.left;
		window->windowSize.Height = rect.bottom - rect.top;

		GetClientRect(win_handle, &rect);
		window->clientSize.Width = rect.right;
		window->clientSize.Height = rect.bottom;

		window->onResizeDelegate(window->clientSize); return 0;
	}
	case WM_SIZING:
	{
		RECT rect;
		GetWindowRect(win_handle, &rect);
		window->windowSize.Width = rect.right - rect.left;
		window->windowSize.Height = rect.bottom - rect.top;

		GetClientRect(win_handle, &rect);
		window->clientSize.Width = rect.right;
		window->clientSize.Height = rect.bottom;

		window->onResizeDelegate(window->clientSize); return 0;
	}
	case WM_MOVING:
	{
		window->onWindowMove(reinterpret_cast<LPRECT>(lParam)->left + window->windowSize.Width / 2, reinterpret_cast<LPRECT>(lParam)->top - window->windowSize.Height / 2); return 0;
	}
	default: return DefWindowProcA(win_handle, uMsg, wParam, lParam);
	}
}

void GTSL::Window::Win32_calculateMousePos(uint32 x, uint32 y)
{
	const auto halfX = static_cast<float>(windowSize.Width) * 0.5f;
	const auto halfY = static_cast<float>(windowSize.Height) * 0.5f;
	mousePos.X = (x - halfX) / halfX; mousePos.Y = (y - halfY) / halfY;
}

void GTSL::Window::Win32_translateKeys(uint64 win32Key, uint64 context, KeyboardKeys& key)
{
	switch (win32Key)
	{
	case VK_BACK:     key = KeyboardKeys::Backspace; return;
	case VK_TAB:      key = KeyboardKeys::Tab; return;
	case VK_RETURN:   key = KeyboardKeys::Enter; return;
	case VK_LSHIFT:   key = KeyboardKeys::LShift; return;
	case VK_RSHIFT:   key = KeyboardKeys::RShift; return;
	case VK_LCONTROL: key = KeyboardKeys::LControl; return;
	case VK_RCONTROL: key = KeyboardKeys::RControl; return;
	case VK_LMENU:    key = KeyboardKeys::Alt; return;
	case VK_RMENU:    key = KeyboardKeys::AltGr; return;
	case VK_CAPITAL:  key = KeyboardKeys::CapsLock; return;
	case VK_UP:       key = KeyboardKeys::UpArrow; return;
	case VK_LEFT:     key = KeyboardKeys::LeftArrow; return;
	case VK_DOWN:     key = KeyboardKeys::DownArrow; return;
	case VK_RIGHT:    key = KeyboardKeys::RightArrow; return;
	case VK_SPACE:    key = KeyboardKeys::SpaceBar; return;
	case 0x30:        key = KeyboardKeys::Keyboard0; return;
	case 0x31:        key = KeyboardKeys::Keyboard1; return;
	case 0x32:        key = KeyboardKeys::Keyboard2; return;
	case 0x33:        key = KeyboardKeys::Keyboard3; return;
	case 0x34:        key = KeyboardKeys::Keyboard4; return;
	case 0x35:        key = KeyboardKeys::Keyboard5; return;
	case 0x36:        key = KeyboardKeys::Keyboard6; return;
	case 0x37:        key = KeyboardKeys::Keyboard7; return;
	case 0x38:        key = KeyboardKeys::Keyboard8; return;
	case 0x39:        key = KeyboardKeys::Keyboard9; return;
	case 0x41:        key = KeyboardKeys::A; return;
	case 0x42:        key = KeyboardKeys::B; return;
	case 0x43:        key = KeyboardKeys::C; return;
	case 0x44:        key = KeyboardKeys::D; return;
	case 0x45:        key = KeyboardKeys::E; return;
	case 0x46:        key = KeyboardKeys::F; return;
	case 0x47:        key = KeyboardKeys::G; return;
	case 0x48:        key = KeyboardKeys::H; return;
	case 0x49:        key = KeyboardKeys::I; return;
	case 0x4A:        key = KeyboardKeys::J; return;
	case 0x4B:        key = KeyboardKeys::K; return;
	case 0x4C:        key = KeyboardKeys::L; return;
	case 0x4D:        key = KeyboardKeys::M; return;
	case 0x4E:        key = KeyboardKeys::N; return;
	case 0x4F:        key = KeyboardKeys::O; return;
	case 0x50:        key = KeyboardKeys::P; return;
	case 0x51:        key = KeyboardKeys::Q; return;
	case 0x52:        key = KeyboardKeys::R; return;
	case 0x53:        key = KeyboardKeys::S; return;
	case 0x54:        key = KeyboardKeys::T; return;
	case 0x55:        key = KeyboardKeys::U; return;
	case 0x56:        key = KeyboardKeys::V; return;
	case 0x57:        key = KeyboardKeys::W; return;
	case 0x58:        key = KeyboardKeys::X; return;
	case 0x59:        key = KeyboardKeys::Y; return;
	case 0x5A:        key = KeyboardKeys::Z; return;
	case VK_F1:       key = KeyboardKeys::F1; return;
	case VK_F2:       key = KeyboardKeys::F2; return;
	case VK_F3:       key = KeyboardKeys::F3; return;
	case VK_F4:       key = KeyboardKeys::F4; return;
	case VK_F5:       key = KeyboardKeys::F5; return;
	case VK_F6:       key = KeyboardKeys::F6; return;
	case VK_F7:       key = KeyboardKeys::F7; return;
	case VK_F8:       key = KeyboardKeys::F8; return;
	case VK_F9:       key = KeyboardKeys::F9; return;
	case VK_F10:      key = KeyboardKeys::F10; return;
	case VK_F11:      key = KeyboardKeys::F11; return;
	case VK_F12:      key = KeyboardKeys::F12; return;
	case VK_NUMPAD0:  key = KeyboardKeys::Numpad0; return;
	case VK_NUMPAD1:  key = KeyboardKeys::Numpad1; return;
	case VK_NUMPAD2:  key = KeyboardKeys::Numpad2; return;
	case VK_NUMPAD3:  key = KeyboardKeys::Numpad3; return;
	case VK_NUMPAD4:  key = KeyboardKeys::Numpad4; return;
	case VK_NUMPAD5:  key = KeyboardKeys::Numpad5; return;
	case VK_NUMPAD6:  key = KeyboardKeys::Numpad6; return;
	case VK_NUMPAD7:  key = KeyboardKeys::Numpad7; return;
	case VK_NUMPAD8:  key = KeyboardKeys::Numpad8; return;
	case VK_NUMPAD9:  key = KeyboardKeys::Numpad9; return;
	case VK_ESCAPE:   key = KeyboardKeys::Esc; return;
	default: return;
	}
}
#endif

GTSL::Window::Window(const WindowCreateInfo& windowCreateInfo)
{
#if (_WIN32)
	WNDCLASSA wndclass{};
	wndclass.lpfnWndProc = reinterpret_cast<WNDPROC>(Win32_windowProc);
	wndclass.cbClsExtra = 0;
	wndclass.hCursor = nullptr;
	wndclass.hIcon = nullptr;
	wndclass.hbrBackground = nullptr;
	wndclass.lpszMenuName = nullptr;
	wndclass.style = 0;
	wndclass.cbWndExtra = 0;

	Application::Win32NativeHandles win32_native_handles;
	windowCreateInfo.Application->GetNativeHandles(&win32_native_handles);
	
	wndclass.hInstance = static_cast<HINSTANCE>(win32_native_handles.HINSTANCE);
	wndclass.lpszClassName = windowCreateInfo.Name.c_str();
	RegisterClassA(&wndclass);
	
	windowHandle = CreateWindowExA(0, wndclass.lpszClassName, windowCreateInfo.Name.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, windowCreateInfo.Extent.Width, windowCreateInfo.Extent.Height, nullptr, nullptr, reinterpret_cast<HINSTANCE>(win32_native_handles.HINSTANCE), nullptr);

	GTSL_ASSERT(!windowHandle, "Window failed to create!")
	
	SetWindowLongPtrA(reinterpret_cast<HWND>(windowHandle), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

	defaultWindowStyle = ::GetWindowLong(reinterpret_cast<HWND>(windowHandle), GWL_STYLE);
#endif
}

GTSL::Window::~Window()
{
	DestroyWindow(reinterpret_cast<HWND>(windowHandle));
}

void GTSL::Window::SetTitle(const char* title)
{
	SetWindowTextA(reinterpret_cast<HWND>(windowHandle), title);
}

void GTSL::Window::Notify()
{
	FlashWindow(reinterpret_cast<HWND>(windowHandle), true);
}

void GTSL::Window::SetIcon(const WindowIconInfo& windowIconInfo)
{
}

void GTSL::Window::GetFramebufferExtent(Extent2D& extent) const
{
	extent = clientSize;
}

void GTSL::Window::SetState(const WindowState& windowState)
{
	switch (windowState.NewWindowSizeState)
	{
	case WindowSizeState::MAXIMIZED:
	{
		//if(windowSizeState == WindowSizeState::FULLSCREEN)
		//{
		//	ChangeDisplaySettingsA(nullptr, CDS_FULLSCREEN);
		//}

		::ShowWindow(reinterpret_cast<HWND>(windowHandle), SW_SHOWMAXIMIZED);

		windowSizeState = windowState.NewWindowSizeState;
	}
	break;

	case WindowSizeState::FULLSCREEN:
	{
		const DWORD remove = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
		const DWORD new_style = defaultWindowStyle & ~remove;
		SetWindowLongPtrA(reinterpret_cast<HWND>(windowHandle), GWL_STYLE, new_style);
		SetWindowPos(reinterpret_cast<HWND>(windowHandle), HWND_TOP, 0, 0, GetSystemMetrics(SM_CXFULLSCREEN), GetSystemMetrics(SM_CXFULLSCREEN), SWP_FRAMECHANGED);
		::ShowWindow(reinterpret_cast<HWND>(windowHandle), SW_SHOWMAXIMIZED);

		//DEVMODEA devmodea;
		//devmodea.dmDisplayFrequency = windowState.RefreshRate;
		//devmodea.dmBitsPerPel = windowState.NewBitsPerPixel;
		//devmodea.dmPelsWidth = windowState.NewResolution.Width;
		//devmodea.dmPelsHeight = windowState.NewResolution.Height;
		//devmodea.
		//ChangeDisplaySettingsA(&devmodea, CDS_FULLSCREEN);

		windowSizeState = windowState.NewWindowSizeState;
	}
	break;

	case WindowSizeState::MINIMIZED:
	{
		//if (windowSizeState == WindowSizeState::FULLSCREEN)
		//{
		//	ChangeDisplaySettingsA(nullptr, CDS_FULLSCREEN);
		//}

		::ShowWindow(reinterpret_cast<HWND>(windowHandle), SW_MINIMIZE);

		windowSizeState = windowState.NewWindowSizeState;
	}
	break;
	}
}

void GTSL::Window::GetNativeHandles(void* nativeHandlesStruct) const
{
	static_cast<Win32NativeHandles*>(nativeHandlesStruct)->HWND = windowHandle;
}

void GTSL::Window::ShowWindow()
{
	::ShowWindow(reinterpret_cast<HWND>(windowHandle), SW_SHOWNORMAL);
}

void GTSL::Window::HideWindow()
{
	::ShowWindow(reinterpret_cast<HWND>(windowHandle), SW_HIDE);
}
