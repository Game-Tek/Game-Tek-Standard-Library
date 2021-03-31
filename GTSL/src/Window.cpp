#include "GTSL/Window.h"

#include <Windows.h>
#include "GTSL/Application.h"
#include "GTSL/Assert.h"
#include "GTSL/StaticString.hpp"

#include "GTSL/Array.hpp"
#include "GTSL/Bitman.h"

#if (_WIN32)
#include "ShObjIdl_core.h"
#include <hidsdi.h>

GTSL::uint64 GTSL::Window::Win32_windowProc(void* hwnd, uint32 uMsg, uint64 wParam, uint64 lParam)
{	
	auto* windowCallData = reinterpret_cast<WindowsCallData*>(GetWindowLongPtrA(static_cast<HWND>(hwnd), GWLP_USERDATA));
	const HWND winHandle = static_cast<HWND>(hwnd);
	if (!windowCallData) { return DefWindowProcA(winHandle, uMsg, wParam, lParam); }
	
	switch (uMsg)
	{
	case WM_CREATE:
	{
		auto* createStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
		windowCallData = static_cast<WindowsCallData*>(createStruct->lpCreateParams);
		SetWindowLongPtrA(static_cast<HWND>(hwnd), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(windowCallData));
		break;
	}
	case WM_CLOSE:
	{
		windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::CLOSE, nullptr);	return 0;
	}
	case WM_LBUTTONDOWN:
	{
		MouseButtonEventData mouseButtonEventData;
		mouseButtonEventData.State = true;
		mouseButtonEventData.Button = MouseButton::LEFT_BUTTON;
		windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::MOUSE_BUTTON, &mouseButtonEventData); return 0;
	}
	case WM_LBUTTONUP:
	{
		MouseButtonEventData mouseButtonEventData;
		mouseButtonEventData.State = false;
		mouseButtonEventData.Button = MouseButton::LEFT_BUTTON;
		windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::MOUSE_BUTTON, &mouseButtonEventData); return 0;
	}
	case WM_RBUTTONDOWN:
	{
		MouseButtonEventData mouseButtonEventData;
		mouseButtonEventData.State = true;
		mouseButtonEventData.Button = MouseButton::RIGHT_BUTTON;
		windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::MOUSE_BUTTON, &mouseButtonEventData); return 0;
	}
	case WM_RBUTTONUP:
	{
		MouseButtonEventData mouseButtonEventData;
		mouseButtonEventData.State = false;
		mouseButtonEventData.Button = MouseButton::RIGHT_BUTTON;
		windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::MOUSE_BUTTON, &mouseButtonEventData); return 0;
	}
	case WM_MBUTTONDOWN:
	{
		MouseButtonEventData mouseButtonEventData;
		mouseButtonEventData.State = true;
		mouseButtonEventData.Button = MouseButton::MIDDLE_BUTTON;
		windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::MOUSE_BUTTON, &mouseButtonEventData); return 0;
	}
	case WM_MBUTTONUP:
	{
		MouseButtonEventData mouseButtonEventData;
		mouseButtonEventData.State = false;
		mouseButtonEventData.Button = MouseButton::MIDDLE_BUTTON;
		windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::MOUSE_BUTTON, &mouseButtonEventData); return 0;
	}
	case WM_KEYDOWN:
	{
		KeyboardKeyEventData keyboardKeyPressEventData;
		Win32_translateKeys(wParam, lParam, keyboardKeyPressEventData.Key);
		keyboardKeyPressEventData.State = true;
		keyboardKeyPressEventData.IsFirstTime = !((lParam >> 30) & 1);
		windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::KEYBOARD_KEY, &keyboardKeyPressEventData); return 0;
	}
	case WM_KEYUP:
	{
		KeyboardKeyEventData keyboardKeyPressEventData;
		Win32_translateKeys(wParam, lParam, keyboardKeyPressEventData.Key);
		keyboardKeyPressEventData.State = false;
		keyboardKeyPressEventData.IsFirstTime = true;
		windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::KEYBOARD_KEY, &keyboardKeyPressEventData); return 0;
	}
	case WM_UNICHAR:
	{
		CharEventData charEventData = wParam;
		windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::CHAR, &charEventData);
		return 0;
	}
	case WM_SIZE:
	{
		WindowSizeEventData clientSize;
		clientSize.Width = LOWORD(lParam);
		clientSize.Height = HIWORD(lParam);

		windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::SIZE, &clientSize);
		return 0;
	}
	case WM_SIZING:
	{
		RECT rect = *reinterpret_cast<RECT*>(lParam);

		if (!windowCallData->hadResize)
		{
			WindowSizeEventData clientSize;
			clientSize.Width = static_cast<uint16>(rect.right);
			clientSize.Height = static_cast<uint16>(rect.bottom);

			windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::SIZE, &clientSize);
			
			windowCallData->hadResize = true;
		}
			
		return true; //An application should return TRUE if it processes this message.
	}
	case WM_MOVING:
	{
		RECT rect;
		Extent2D windowSize;
		GetWindowRect(winHandle, &rect);
		windowSize.Width = static_cast<uint16>(rect.right);
		windowSize.Height = static_cast<uint16>(rect.bottom);

		WindowMoveEventData windowMoveEventData;
		windowMoveEventData.X = reinterpret_cast<LPRECT>(lParam)->left + windowSize.Width / 2;
		windowMoveEventData.Y = reinterpret_cast<LPRECT>(lParam)->top - windowSize.Height / 2;
			
		windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::MOVING, &windowMoveEventData);
		return true; //An application should return TRUE if it processes this message.
	}
	case WM_ACTIVATE:
	{
		auto hasFocus = static_cast<bool>(LOWORD(wParam));
		FocusEventData focus;
		focus.Focus = hasFocus;
		focus.HadFocus = windowCallData->WindowPointer->focus;
		windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::FOCUS, static_cast<FocusEventData*>(&focus));
		windowCallData->WindowPointer->focus = hasFocus;
		return 0;
	}
	case WM_SETFOCUS:
	{
		FocusEventData focus;
		focus.Focus = true;
		focus.HadFocus = windowCallData->WindowPointer->focus;
		windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::FOCUS, static_cast<FocusEventData*>(&focus));
		windowCallData->WindowPointer->focus = true;
		return 0;
	}
	case WM_KILLFOCUS:
	{
		FocusEventData focus;
		focus.Focus = false;
		focus.HadFocus = windowCallData->WindowPointer->focus;
		windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::FOCUS, static_cast<FocusEventData*>(&focus));
		windowCallData->WindowPointer->focus = false;
		return 0;
	}
	case WM_INPUT:
	{
		PRAWINPUT pRawInput; UINT dataSize; HANDLE hHeap = GetProcessHeap();

		GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, nullptr, &dataSize, sizeof(RAWINPUTHEADER));

		pRawInput = (PRAWINPUT)HeapAlloc(hHeap, 0, dataSize);
		if (!pRawInput)
			return 0;

		GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, pRawInput, &dataSize, sizeof(RAWINPUTHEADER));

		switch (pRawInput->header.dwType)
		{
		case RIM_TYPEMOUSE: //mouse
		{
			MouseMoveEventData result;
			int32 x = pRawInput->data.mouse.lLastX, y = pRawInput->data.mouse.lLastY;
				
			if((pRawInput->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE) == MOUSE_MOVE_ABSOLUTE)
			{
				bool isVirtualDesktop = (pRawInput->data.mouse.usFlags & MOUSE_VIRTUAL_DESKTOP) == MOUSE_VIRTUAL_DESKTOP;

				int width = GetSystemMetrics(isVirtualDesktop ? SM_CXVIRTUALSCREEN : SM_CXSCREEN);
				int height = GetSystemMetrics(isVirtualDesktop ? SM_CYVIRTUALSCREEN : SM_CYSCREEN);

				int absoluteX = int((pRawInput->data.mouse.lLastX / 65535.0f) * width);
				int absoluteY = int((pRawInput->data.mouse.lLastY / 65535.0f) * height);

				result.X() = absoluteX; result.Y() = absoluteY;
			}

			if((pRawInput->data.mouse.usFlags & MOUSE_MOVE_RELATIVE) == MOUSE_MOVE_RELATIVE)
			{
				result.X() = static_cast<float32>(x); result.Y() = static_cast<float32>(-y);
			}

			windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::MOUSE_MOVE, &result);
				
			// Wheel data needs to be pointer casted to interpret an unsigned short as a short, with no conversion
			// otherwise it'll overflow when going negative.
			// Didn't happen before some minor changes in the code, doesn't seem to go away
			// so it's going to have to be like this.
			if (pRawInput->data.mouse.usButtonFlags & RI_MOUSE_WHEEL) {
				MouseWheelEventData mouseWheelEventData = -static_cast<float32>(*reinterpret_cast<short*>(&pRawInput->data.mouse.usButtonData)) / static_cast<float32>(WHEEL_DELTA);
				windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::MOUSE_WHEEL, &mouseWheelEventData);
			}
				
			break;
		}
			
		case RIM_TYPEHID: //controller
		{
			GetRawInputDeviceInfoA(pRawInput->header.hDevice, RIDI_PREPARSEDDATA, nullptr, &dataSize);
			auto pPreparsedData = (PHIDP_PREPARSED_DATA)HeapAlloc(hHeap, 0, dataSize);
			GetRawInputDeviceInfoA(pRawInput->header.hDevice, RIDI_PREPARSEDDATA, pPreparsedData, &dataSize);

			HIDP_CAPS caps;

			HidP_GetCaps(pPreparsedData, &caps);

			auto pButtonCaps = (PHIDP_BUTTON_CAPS)HeapAlloc(hHeap, 0, sizeof(HIDP_BUTTON_CAPS) * caps.NumberInputButtonCaps);

			{
				auto capsLength = caps.NumberInputButtonCaps;
				HidP_GetButtonCaps(HidP_Input, pButtonCaps, &capsLength, pPreparsedData);
			}

			auto g_NumberOfButtons = pButtonCaps->Range.UsageMax - pButtonCaps->Range.UsageMin + 1;

			auto pValueCaps = (PHIDP_VALUE_CAPS)HeapAlloc(hHeap, 0, sizeof(HIDP_VALUE_CAPS) * caps.NumberInputValueCaps);

			{
				auto capsLength = caps.NumberInputValueCaps;
				HidP_GetValueCaps(HidP_Input, pValueCaps, &capsLength, pPreparsedData);
			}

			USAGE usage[512]; //TODO: ASSERT

			ULONG usageLength = g_NumberOfButtons;
			HidP_GetUsages(HidP_Input, pButtonCaps->UsagePage, 0, usage, &usageLength, pPreparsedData, (PCHAR)pRawInput->data.hid.bRawData, pRawInput->data.hid.dwSizeHid);

			bool buttonStates[512]{ false };

			for (uint32 i = 0; i < usageLength; i++) {
				buttonStates[usage[i] - pButtonCaps->Range.UsageMin] = true;
			}

			ULONG value; LONG lAxisX, lAxisY, lAxisZ, lAxisRz, lHat;

			for (uint32 i = 0; i < caps.NumberInputValueCaps; i++)
			{
				HidP_GetUsageValue(HidP_Input, pValueCaps[i].UsagePage, 0, pValueCaps[i].Range.UsageMin, &value, pPreparsedData, (PCHAR)pRawInput->data.hid.bRawData, pRawInput->data.hid.dwSizeHid);

				switch (pValueCaps[i].Range.UsageMin)
				{
				case 0x30:    // X-axis
					lAxisX = (LONG)value - 128;
					break;

				case 0x31:    // Y-axis
					lAxisY = (LONG)value - 128;
					break;

				case 0x32: // Z-axis
					lAxisZ = (LONG)value - 128;
					break;

				case 0x35: // Rotate-Z
					lAxisRz = (LONG)value - 128;
					break;

				case 0x39:    // Hat Switch
					lHat = value;
					break;
				}
			}

			break;
		}
		}
			
		HeapFree(hHeap, 0, pRawInput);
		
		return 0;
	}
	}

	return DefWindowProcA(winHandle, uMsg, wParam, lParam);
}

void GTSL::Window::Win32_calculateMousePos(const uint32 x, const uint32 y, GTSL::Extent2D clientSize, GTSL::Vector2& mousePos)
{
	const auto halfX = static_cast<float>(clientSize.Width) * 0.5f;
	const auto halfY = static_cast<float>(clientSize.Height) * 0.5f;
	mousePos.X() = (x - halfX) / halfX; mousePos.Y() = (halfY - y) / halfY;
}

void GTSL::Window::Win32_translateKeys(const uint64 win32Key, uint64 context, KeyboardKeys& key)
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

void GTSL::Window::BindToOS(const WindowCreateInfo& windowCreateInfo)
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
	wndclass.lpszClassName = windowCreateInfo.Name.begin();

	Application::Win32NativeHandles win32NativeHandles;
	windowCreateInfo.Application->GetNativeHandles(&win32NativeHandles);
	
	wndclass.hInstance = static_cast<HINSTANCE>(win32NativeHandles.HINSTANCE);
	RegisterClassA(&wndclass);

	uint32 style = 0;
	switch (windowCreateInfo.Type)
	{
	case WindowType::OS_WINDOW: style = WS_OVERLAPPEDWINDOW; break;
	case WindowType::NO_ELEMENTS: style = WS_POPUP; break;
	}

	WindowsCallData windowsCallData;
	windowsCallData.UserData = windowCreateInfo.UserData;
	windowsCallData.WindowPointer = this;
	windowsCallData.FunctionToCall = windowCreateInfo.Function;

	RECT windowRect;
	windowRect.top = 0; windowRect.left = 0;
	windowRect.bottom = windowCreateInfo.Extent.Height; windowRect.right = windowCreateInfo.Extent.Width;
	AdjustWindowRect(&windowRect, style, false);
	windowHandle = CreateWindowExA(0, wndclass.lpszClassName, windowCreateInfo.Name.begin(), style, CW_USEDEFAULT, CW_USEDEFAULT, windowRect.right, windowRect.bottom, nullptr, nullptr, static_cast<HINSTANCE>(win32NativeHandles.HINSTANCE), &windowsCallData);

	GTSL_ASSERT(windowHandle, "Window failed to create!");

	defaultWindowStyle = GetWindowLongA(static_cast<HWND>(windowHandle), GWL_STYLE);

	//CoCreateInstance(IID_ITaskbarList3, nullptr, CLSCTX::CLSCTX_INPROC_SERVER, IID_ITaskbarList, &iTaskbarList);
#endif
}

GTSL::Window::~Window()
{
	DestroyWindow(static_cast<HWND>(windowHandle));
}

void GTSL::Window::Update(void* userData, Delegate<void(void*, WindowEvents, void*)> function)
{
	Window::WindowsCallData windowsCallData;
	windowsCallData.WindowPointer = this;
	windowsCallData.UserData = userData;
	windowsCallData.FunctionToCall = function;
	SetWindowLongPtrA(static_cast<HWND>(windowHandle), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&windowsCallData));

	MSG message;

	while (PeekMessageA(&message, static_cast<HWND>(windowHandle), 0, 0, PM_REMOVE)) {
		TranslateMessage(&message);
		DispatchMessageA(&message);
	}
	
	SetWindowLongPtrA(static_cast<HWND>(windowHandle), GWLP_USERDATA, 0);
}

void GTSL::Window::SetTitle(const char* title)
{
	SetWindowTextA(static_cast<HWND>(windowHandle), title);
}

void GTSL::Window::Notify()
{
	FlashWindow(static_cast<HWND>(windowHandle), true);
}

void GTSL::Window::SetIcon(const WindowIconInfo& windowIconInfo)
{
}

GTSL::Extent2D GTSL::Window::GetFramebufferExtent() const
{
	RECT rect; GTSL::Extent2D extent;
	GetClientRect(static_cast<HWND>(windowHandle), &rect);
	extent.Width = static_cast<uint16>(rect.right);
	extent.Height = static_cast<uint16>(rect.bottom);
	return extent;
}

GTSL::Extent2D GTSL::Window::GetWindowExtent() const
{
	RECT rect; GTSL::Extent2D extent;
	GetWindowRect(static_cast<HWND>(windowHandle), &rect);
	extent.Width = static_cast<uint16>(rect.right - rect.left);
	extent.Height = static_cast<uint16>(rect.bottom - rect.top);
	return extent;
}

void GTSL::Window::SetMousePosition(const Extent2D position)
{
	SetCursorPos(position.Width, position.Height);
}

void GTSL::Window::LimitMousePosition(Extent2D range)
{
	RECT rect;
	GetClientRect(static_cast<HWND>(windowHandle), &rect);
	ClipCursor(&rect);
}

void GTSL::Window::ShowMouse(const bool show) { ShowCursor(show); }

void GTSL::Window::SetState(const WindowState& windowState)
{
	switch (windowState.NewWindowSizeState)
	{
	case WindowSizeState::MAXIMIZED:
	{
		SetWindowLongPtrA(static_cast<HWND>(windowHandle), GWL_STYLE, defaultWindowStyle);
		::ShowWindowAsync(static_cast<HWND>(windowHandle), SW_SHOWMAXIMIZED);

		windowSizeState = windowState.NewWindowSizeState;
	}
	break;

	case WindowSizeState::FULLSCREEN:
	{
		const DWORD remove = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
		const DWORD new_style = defaultWindowStyle & ~remove;
		SetWindowLongPtrA(static_cast<HWND>(windowHandle), GWL_STYLE, new_style);
		SetWindowPos(static_cast<HWND>(windowHandle), HWND_TOP, 0, 0, GetSystemMetrics(SM_CXFULLSCREEN), GetSystemMetrics(SM_CYFULLSCREEN), 0);
		::ShowWindowAsync(static_cast<HWND>(windowHandle), SW_SHOWMAXIMIZED);

		windowSizeState = windowState.NewWindowSizeState;
	}
	break;

	case WindowSizeState::MINIMIZED:
	{
		SetWindowLongPtrA(static_cast<HWND>(windowHandle), GWL_STYLE, defaultWindowStyle);
		::ShowWindowAsync(static_cast<HWND>(windowHandle), SW_MINIMIZE);

		windowSizeState = windowState.NewWindowSizeState;
	}
	break;
	}
}

void GTSL::Window::GetNativeHandles(void* nativeHandlesStruct) const
{
	static_cast<Win32NativeHandles*>(nativeHandlesStruct)->HWND = windowHandle;
}

// Call async version to ensure window proc does not get called at the moment this is called, as we will not have a valid window proc state set

void GTSL::Window::ShowWindow()
{
	::ShowWindowAsync(static_cast<HWND>(windowHandle), SW_SHOW);
}

void GTSL::Window::HideWindow()
{
	::ShowWindowAsync(static_cast<HWND>(windowHandle), SW_HIDE);
}

void GTSL::Window::AddDevice(const DeviceType deviceType)
{
	Array<RAWINPUTDEVICE, 8> rawInputDevices;
	
	switch (deviceType)
	{
		case DeviceType::MOUSE:
		{
			RAWINPUTDEVICE rid;
			rid.usUsagePage = HID_USAGE_PAGE_GENERIC; //generic usage
			rid.usUsage = HID_USAGE_GENERIC_MOUSE; // MOUSE
			rid.dwFlags = RIDEV_INPUTSINK;
			rid.hwndTarget = (HWND)windowHandle;
			rawInputDevices.EmplaceBack(rid);
			break;
		}
		
		case DeviceType::GAMEPAD:
		{
			RAWINPUTDEVICE rid;
			rid.usUsagePage = HID_USAGE_PAGE_GENERIC;
			rid.usUsage = HID_USAGE_GENERIC_JOYSTICK; // Joystick
			rid.dwFlags = 0;
			rid.hwndTarget = (HWND)windowHandle;
			rawInputDevices.EmplaceBack(rid);
			break;
		}
	}

	if (!RegisterRawInputDevices(rawInputDevices.begin(), rawInputDevices.GetLength(), sizeof(RAWINPUTDEVICE)))
	{
		auto errorCode = GetLastError();
		GTSL_ASSERT(false, "Failed to register devices");
	}
}

void GTSL::Window::SetProgressState(ProgressState progressState) const
{
	TBPFLAG flags = TBPF_NOPROGRESS;

#undef ERROR
	
	switch (progressState)
	{
	case ProgressState::NONE: break;
	case ProgressState::INDETERMINATE: flags = TBPF_INDETERMINATE; break;
	case ProgressState::NORMAL: flags = TBPF_NORMAL; break;
	case ProgressState::PAUSED: flags = TBPF_PAUSED; break;
	case ProgressState::ERROR: flags = TBPF_ERROR; break;
	default: flags = TBPF_NOPROGRESS;
	}

	//static_cast<ITaskbarList3*>(iTaskbarList)->SetProgressState(static_cast<HWND>(windowHandle), flags);
}

void GTSL::Window::SetProgressValue(const float32 value) const
{
	//static_cast<ITaskbarList3*>(iTaskbarList)->SetProgressValue(static_cast<HWND>(windowHandle), value * 1000u, 1000u);
}
