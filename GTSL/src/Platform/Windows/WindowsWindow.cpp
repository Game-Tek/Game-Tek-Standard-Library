#include "GTSL/Platform/Windows/WindowsWindow.h"

#include "GTSL/Platform/Windows/WindowsApplication.h"

using namespace GTSL;

uint64 WindowsWindow::WindowProc(const HWND hwnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam)
{
	const auto windows_window = reinterpret_cast<WindowsWindow*>(GetWindowLongPtrA(hwnd, GWLP_USERDATA));

	switch (uMsg)
	{
	case WM_CLOSE:
	{
		windows_window->onCloseDelegate();	return 0;
	}
	case WM_MOUSEMOVE: /*absolute pos*/
	{
		const auto mouse_x = windows_window->mouseX, mouse_y = windows_window->mouseY;
		windows_window->CalculateMousePos(LOWORD(lParam), HIWORD(lParam));
		windows_window->onMouseMove(windows_window->mouseX, windows_window->mouseY, windows_window->mouseX - mouse_x, windows_window->mouseY - mouse_y); return 0;
	}
	case WM_MOUSEHWHEEL:
	{
		windows_window->onMouseWheelMove(GET_WHEEL_DELTA_WPARAM(wParam)); return 0;
	}
	case WM_LBUTTONDOWN:
	{
		windows_window->onMouseButtonClick(MouseButton::LEFT_BUTTON, MouseButtonState::PRESSED);  return 0;
	}
	case WM_LBUTTONUP:
	{
		windows_window->onMouseButtonClick(MouseButton::LEFT_BUTTON, MouseButtonState::RELEASED); return 0;
	}
	case WM_RBUTTONDOWN:
	{
		windows_window->onMouseButtonClick(MouseButton::RIGHT_BUTTON, MouseButtonState::PRESSED);  return 0;
	}
	case WM_RBUTTONUP:
	{
		windows_window->onMouseButtonClick(MouseButton::RIGHT_BUTTON, MouseButtonState::RELEASED); return 0;
	}
	case WM_MBUTTONDOWN:
	{
		windows_window->onMouseButtonClick(MouseButton::MIDDLE_BUTTON, MouseButtonState::PRESSED);  return 0;
	}
	case WM_MBUTTONUP:
	{
		windows_window->onMouseButtonClick(MouseButton::MIDDLE_BUTTON, MouseButtonState::RELEASED); return 0;
	}
	case WM_KEYDOWN:
	{
		KeyboardKeys keyboard_keys;
		TranslateKeys(wParam, lParam, keyboard_keys);
		windows_window->onKeyEvent(keyboard_keys, KeyboardKeyState::PRESSED);  return 0;
	}
	case WM_KEYUP:
	{
		KeyboardKeys keyboard_keys;
		TranslateKeys(wParam, lParam, keyboard_keys);
		windows_window->onKeyEvent(keyboard_keys, KeyboardKeyState::RELEASED); return 0;
	}
	case WM_CHAR:
	{
		windows_window->onCharEvent(wParam);
	}
	case WM_SIZE:
	{
		RECT rect;
		GetWindowRect(windows_window->windowHandle, &rect);
		windows_window->windowSize.Width = rect.right - rect.left;
		windows_window->windowSize.Height = rect.bottom - rect.top;

		GetClientRect(windows_window->windowHandle, &rect);
		windows_window->clientSize.Width = rect.right;
		windows_window->clientSize.Height = rect.bottom;
			
		windows_window->onWindowResize(windows_window->clientSize.Width, windows_window->clientSize.Height); return 0;
	}
	case WM_SIZING:
	{
		RECT rect;
		GetWindowRect(windows_window->windowHandle, &rect);
		windows_window->windowSize.Width = rect.right - rect.left;
		windows_window->windowSize.Height = rect.bottom - rect.top;

		GetClientRect(windows_window->windowHandle, &rect);
		windows_window->clientSize.Width = rect.right;
		windows_window->clientSize.Height = rect.bottom;
			
		windows_window->onWindowResize(windows_window->clientSize.Width, windows_window->clientSize.Height); return true;
	}
	case WM_MOVING:
	{
		windows_window->onWindowMove(reinterpret_cast<LPRECT>(lParam)->left + windows_window->windowSize.Width / 2, reinterpret_cast<LPRECT>(lParam)->top - windows_window->windowSize.Height / 2); return true;
	}
	default: return 0;
	}
}

void WindowsWindow::CalculateMousePos(const uint32 x, const uint32 y)
{
	const auto halfX = static_cast<float>(windowSize.Width) * 0.5f;
	const auto halfY = static_cast<float>(windowSize.Height) * 0.5f;
	mouseX = (x - halfX) / halfX; mouseY = (y - halfY) / halfY;
}

void WindowsWindow::TranslateKeys(uint32 win32Key, uint64 context, KeyboardKeys& key)
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

WindowsWindow::WindowsWindow(const WindowCreateInfo& windowCreateInfo) : Window(windowCreateInfo)
{
	WNDCLASSA wndclass{};
	wndclass.lpfnWndProc = reinterpret_cast<WNDPROC>(WindowProc);
	wndclass.hInstance = static_cast<WindowsApplication*>(windowCreateInfo.Application)->GetInstance();
	wndclass.lpszClassName = windowCreateInfo.Name.c_str();
	wndclass.hCursor = nullptr;
	RegisterClassA(&wndclass);
	
	windowHandle = CreateWindowExA(0, wndclass.lpszClassName, windowCreateInfo.Name.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, windowCreateInfo.Extent.Width, windowCreateInfo.Extent.Height, static_cast<WindowsWindow*>(windowCreateInfo.ParentWindow)->windowHandle, nullptr, static_cast<WindowsApplication*>(windowCreateInfo.Application)->GetInstance(), nullptr);

	SetWindowLongPtrA(windowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

	defaultWindowStyle = ::GetWindowLong(windowHandle, GWL_STYLE);
}

void WindowsWindow::SetState(const WindowState& windowState)
{
	switch (windowState.NewWindowSizeState)
	{
	case WindowSizeState::MAXIMIZED:
		{
			//if(windowSizeState == WindowSizeState::FULLSCREEN)
			//{
			//	ChangeDisplaySettingsA(nullptr, CDS_FULLSCREEN);
			//}
			
			ShowWindow(windowHandle, SW_SHOWMAXIMIZED);

			windowSizeState = windowState.NewWindowSizeState;
		}
		break;
		
	case WindowSizeState::FULLSCREEN:
		{
			const DWORD remove = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
			const DWORD new_style = defaultWindowStyle & ~remove;
			SetWindowLongPtrA(windowHandle, GWL_STYLE, new_style);
			SetWindowPos(windowHandle, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXFULLSCREEN), GetSystemMetrics(SM_CXFULLSCREEN), SWP_FRAMECHANGED);
			ShowWindow(windowHandle, SW_SHOWMAXIMIZED);

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

			ShowWindow(windowHandle, SW_MINIMIZE);

			windowSizeState = windowState.NewWindowSizeState;
		}
		break;
	}
}

void WindowsWindow::GetNativeHandles(void* nativeHandlesStruct)
{
	static_cast<Win32NativeHandles*>(nativeHandlesStruct)->HWND = windowHandle;
}

void WindowsWindow::Notify()
{
	FlashWindow(windowHandle, true);
}

void WindowsWindow::SetTitle(const char* title)
{
	SetWindowTextA(windowHandle, title);
}