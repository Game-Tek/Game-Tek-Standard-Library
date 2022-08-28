#pragma once

#include "Extent.h"

#include "Delegate.hpp"
#include "Gamepad.h"
#include "Range.hpp"
#include "Vector.hpp"
#include "Math/Vectors.hpp"
#include "StringCommon.h"

#if (_WIN64)
#define WIN32_LEAN_AND_MEAN
#include "ShObjIdl_core.h"
#include <WinUser.h>
#include <hidsdi.h>
#include <SetupAPI.h>
#include <Dbt.h>
#undef ERROR
#elif __linux__
#include <X11/X.h>
#include <xcb/xcb.h>
#endif

namespace GTSL
{
	class Window {
	public:
		enum class MouseButton : uint8 {
			LEFT_BUTTON, RIGHT_BUTTON, MIDDLE_BUTTON
		};

		enum class KeyboardKeys : uint8 {
			Q, W, E, R, T, Y, U, I, O, P, A, S, D, F, G, H, J, K, L, Z, X, C, V, B, N, M,

			Keyboard0, Keyboard1, Keyboard2, Keyboard3, Keyboard4, Keyboard5, Keyboard6, Keyboard7, Keyboard8, Keyboard9,

			Backspace,
			Enter,

			Supr,

			Tab,
			CapsLock,

			Esc,

			RShift,
			LShift,

			RControl,
			LControl,

			Alt,
			AltGr,

			UpArrow,
			RightArrow,
			DownArrow,
			LeftArrow,

			SpaceBar,

			Numpad0, Numpad1, Numpad2, Numpad3, Numpad4, Numpad5, Numpad6, Numpad7, Numpad8, Numpad9,

			F1,
			F2,
			F3,
			F4,
			F5,
			F6,
			F7,
			F8,
			F9,
			F10,
			F11,
			F12
		};

		enum class WindowType : uint8 {
			OS_WINDOW, NO_ELEMENTS
		};

		enum class WindowSizeState : uint8 {
			MINIMIZED, MAXIMIZED, FULLSCREEN
		};
		
#if _WIN64
		Window() = default;
#elif __linux__
		Window() : connection(nullptr), window(0), screen(nullptr) {}
#endif

		struct JoystickUpdate {
			Gamepad::Side Side;
			Gamepad::SourceNames Source;

			union {
				Vector2 Vector;
				float32 Linear;
				bool Action;
			};
		};

		enum class WindowEvents {
			CLOSE, KEYBOARD_KEY, CHAR, SIZE, MOVING, MOUSE_MOVE, MOUSE_WHEEL,
			MOUSE_BUTTON, FOCUS, JOYSTICK_UPDATE, DEVICE_CHANGE, PPI_CHANGE
		};

		struct MouseButtonEventData {
			MouseButton Button; bool State;
		};

		struct KeyboardKeyEventData {
			KeyboardKeys Key; bool State; bool IsFirstTime;
		};

		using CharEventData = uint32;

		struct WindowMoveEventData {
			uint16 X, Y;
		};

		/**
		 * \brief Delegate called when mouse moves, the first two floats are the X;Y in the -1 <-> 1 range, and the other two floats are delta position in the same range in respect to the last update to the screen.
		 */
		using MouseMoveEventData = Vector2;
		using WindowSizeEventData = Extent2D;
		using MouseWheelEventData = float32;

		struct FocusEventData {
			bool Focus, HadFocus;
		};

		struct PPIChangeData {
			uint32 PPI = 0;
		};

		struct WindowsCallData {
			void* UserData;
			Delegate<void(void*, WindowEvents, void*)> FunctionToCall;
			Window* WindowPointer;
			bool hadResize = false;
			MouseMoveEventData MouseMove;
		};

		struct DeviceChangeData {
			bool isController = false;
		};
		
		void BindToOS(StringView name, Extent2D extent, void* userData, Delegate<void(void*, WindowEvents, void*)> function, Window parentWindow = Window(), WindowType type = WindowType::OS_WINDOW) {
#if (_WIN64)
			WNDCLASSA wndclass{};
			wndclass.lpfnWndProc = reinterpret_cast<WNDPROC>(Win32_windowProc);
			wndclass.cbClsExtra = 0;
			wndclass.hCursor = nullptr;
			wndclass.hIcon = nullptr;
			wndclass.hbrBackground = nullptr;
			wndclass.lpszMenuName = nullptr;
			wndclass.style = 0;
			wndclass.cbWndExtra = 0;
			wndclass.lpszClassName = reinterpret_cast<const char*>(name.GetData());
			wndclass.hInstance = GetModuleHandle(nullptr);
			RegisterClassA(&wndclass);

			uint32 style = 0;
			switch (type) {
			case WindowType::OS_WINDOW: style = WS_OVERLAPPEDWINDOW; break;
			case WindowType::NO_ELEMENTS: style = WS_POPUP; break;
			}

			WindowsCallData windowsCallData;
			windowsCallData.UserData = userData;
			windowsCallData.WindowPointer = this;
			windowsCallData.FunctionToCall = function;

			RECT windowRect;
			windowRect.top = 0; windowRect.left = 0;
			windowRect.bottom = extent.Height; windowRect.right = extent.Width;
			AdjustWindowRect(&windowRect, style, false);
			windowHandle = CreateWindowExA(0, wndclass.lpszClassName, reinterpret_cast<const char*>(name.GetData()), style, CW_USEDEFAULT, CW_USEDEFAULT, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, nullptr, nullptr, GetModuleHandle(nullptr), &windowsCallData);

			GTSL_ASSERT(windowHandle, "Window failed to create!");

			defaultWindowStyle = GetWindowLongA(windowHandle, GWL_STYLE);

			//CoCreateInstance(IID_ITaskbarList3, nullptr, CLSCTX::CLSCTX_INPROC_SERVER, IID_ITaskbarList, &iTaskbarList);
#elif __linux__
			int screenp = 0;

			connection = xcb_connect(nullptr, &screenp);

			if(xcb_connection_has_error(connection) != 0) {
				return;
			}

			xcb_screen_iterator_t iter = xcb_setup_roots_iterator(xcb_get_setup(connection));

			for(uint32 i = screenp; i > 0; --i) {
				xcb_screen_next(&iter);
			}

			screen = iter.data;

			window = xcb_generate_id(connection);

			uint32 eventMask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;

			uint32 valueList[] = { screen->black_pixel, 0u };

			xcb_create_window(connection, XCB_COPY_FROM_PARENT, window, screen->root, 0, 0, extent.Width, extent.Height, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, eventMask, valueList);

			SetTitle(name);

			xcb_intern_atom_cookie_t wmDeleteCookie = xcb_intern_atom(connection, 0, strlen("WM_DELETE_WINDOW"), "WM_DELETE_WINDOW");
			xcb_intern_atom_cookie_t wmProtocolsCookie = xcb_intern_atom(connection, 0, strlen("WM_PROTOCOLS"), "WM_PROTOCOLS");
			xcb_intern_atom_reply_t *wmDeleteReply = xcb_intern_atom_reply(connection, wmDeleteCookie, nullptr);
			xcb_intern_atom_reply_t *wmProtocolsReply = xcb_intern_atom_reply(connection, wmProtocolsCookie, nullptr);
			wmDeleteWin = wmDeleteReply->atom;
			wmProtocols = wmProtocolsReply->atom;

			xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, wmProtocolsReply->atom, 4, 32, 1, &wmDeleteReply->atom);

			this->framebufferExtent = extent;
#endif
		}
		
		~Window() {
#if (_WIN64)
			DestroyWindow(windowHandle);
#elif __linux__
			if(connection && window) {
				xcb_destroy_window(connection, window);
			}
#endif
		}

		void Update(void* userData, Delegate<void(void*, WindowEvents, void*)> function) {
#if (_WIN64)
			WindowsCallData windowsCallData;
			windowsCallData.WindowPointer = this;
			windowsCallData.UserData = userData;
			windowsCallData.FunctionToCall = function;
			windowsCallData.MouseMove = MouseMoveEventData{ 0 };
			SetWindowLongPtrA(windowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&windowsCallData));

			MSG message;

			while (PeekMessageA(&message, windowHandle, 0, 0, PM_REMOVE)) {
				TranslateMessage(&message);
				DispatchMessageA(&message);
			}

			SetWindowLongPtrA(windowHandle, GWLP_USERDATA, 0);

			if(windowsCallData.MouseMove != 0) {
				function(userData, WindowEvents::MOUSE_MOVE, &windowsCallData.MouseMove);
			}
#elif __linux__
			xcb_generic_event_t* event = nullptr;
			xcb_client_message_event_t* clientMessage = nullptr;
			
			while(event = xcb_poll_for_event(connection)) {
				switch (event->response_type & ~0x80) {
				case XCB_CLIENT_MESSAGE: {
					clientMessage = (xcb_client_message_event_t *)event;

					if (clientMessage->data.data32[0] == wmDeleteWin) {
						// ON delete window
					}

					break;
				}
				}

				free(event);
			}
#endif
		}
		
		void SetTitle(const GTSL::StringView title) {
#if (_WIN64)
			SetWindowTextA(windowHandle, (LPCSTR)title.GetData());
#elif __linux__
			xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, title.GetBytes(), title.GetData());
#endif
		}
		
		void Notify() {
#if (_WIN64)
			FlashWindow(windowHandle, true);
#elif __linux__
#endif
		}

#if (_WIN64)
		HWND GetHWND() const { return windowHandle; }
#elif __linux__
		auto GetXCBConnection() const { return connection; }
		auto GetXCBWindow() const { return window; }
#endif

		struct WindowIconInfo {
			byte* Data = nullptr;
			Extent2D Extent;
		};
		void SetIcon(const WindowIconInfo & windowIconInfo);

		Extent2D GetFramebufferExtent() const {
#if (_WIN64)
			RECT rect; GTSL::Extent2D extent;
			GetClientRect(windowHandle, &rect);
			extent.Width = static_cast<uint16>(rect.right);
			extent.Height = static_cast<uint16>(rect.bottom);
			return extent;
#elif __linux__
			return framebufferExtent;
#endif
		}
		
		Extent2D GetWindowExtent() const {
#if (_WIN64)
			RECT rect; GTSL::Extent2D extent;
			GetWindowRect(windowHandle, &rect);
			extent.Width = static_cast<uint16>(rect.right - rect.left);
			extent.Height = static_cast<uint16>(rect.bottom - rect.top);
			return extent;
#elif __linux__
#endif
		}

		void SetMousePosition(Extent2D position) {
#if (_WIN64)
			SetCursorPos(position.Width, position.Height);
#elif __linux__
#endif
		}
		
		void LimitMousePosition(Extent2D range) {
#if (_WIN64)
			RECT rect;
			GetClientRect(windowHandle, &rect);
			ClipCursor(&rect);
#elif __linux__
#endif
		}

		void ShowMouse(bool show) {
#if (_WIN64)
			ShowCursor(show);
#elif __linux__
#endif
		}
		
		static void GetAspectRatio(const Extent2D & extent, float& aspectRatio) { aspectRatio = static_cast<float>(extent.Width) / static_cast<float>(extent.Height); }

		struct WindowState
		{
			WindowSizeState NewWindowSizeState;
			uint32 RefreshRate{ 0 };
			Extent2D NewResolution;
			uint8 NewBitsPerPixel = 8;
		};
		void SetState(const WindowState & windowState) {
	#if (_WIN64)
			switch (windowState.NewWindowSizeState) {
			case WindowSizeState::MAXIMIZED: {
				SetWindowLongPtrA(windowHandle, GWL_STYLE, defaultWindowStyle);
				ShowWindowAsync(windowHandle, SW_SHOWMAXIMIZED);

				windowSizeState = windowState.NewWindowSizeState;
				break;
			}
			case WindowSizeState::FULLSCREEN: {
				const DWORD remove = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
				const DWORD new_style = defaultWindowStyle & ~remove;
				SetWindowLongPtrA(windowHandle, GWL_STYLE, new_style);
				SetWindowPos(windowHandle, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXFULLSCREEN), GetSystemMetrics(SM_CYFULLSCREEN), 0);
				ShowWindowAsync(windowHandle, SW_SHOWMAXIMIZED);

				windowSizeState = windowState.NewWindowSizeState;
				break;
			}
			case WindowSizeState::MINIMIZED: {
				SetWindowLongPtrA(windowHandle, GWL_STYLE, defaultWindowStyle);
				ShowWindowAsync(windowHandle, SW_MINIMIZE);

				windowSizeState = windowState.NewWindowSizeState;
				break;
			}
			}
#elif __linux__
#endif
		}

		struct NativeHandles {
#if (_WIN64)
			HWND HWND{ nullptr };
#elif __linux__
			xcb_connection_t* Connection = nullptr;
			xcb_window_t Window = 0;
#endif
		};
		void GetNativeHandles(NativeHandles* nativeHandlesStruct) const {
#if (_WIN64)
			nativeHandlesStruct->HWND = windowHandle;
#elif __linux__
			nativeHandlesStruct->Connection = connection;
			nativeHandlesStruct->Window = window;
#endif
		}

		void SetWindowVisibility(bool visible) {
#if (_WIN64)
			// Call async version to ensure window proc does not get called at the moment this is called, as we will not have a valid window proc state set
			ShowWindowAsync(windowHandle, visible ? SW_SHOW : SW_HIDE);
#elif __linux__
			xcb_map_window(connection, window);
			xcb_flush(connection);
#endif
		}

		enum class DeviceType : uint8 { MOUSE, GAMEPAD };
		
		void AddDevice(const DeviceType deviceType) {
#if (_WIN64)
			StaticVector<RAWINPUTDEVICE, 8> rawInputDevices;

			switch (deviceType) {
			case DeviceType::MOUSE: {
				RAWINPUTDEVICE rid;
				rid.usUsagePage = HID_USAGE_PAGE_GENERIC; //generic usage
				rid.usUsage = HID_USAGE_GENERIC_MOUSE;
				rid.dwFlags = 0;
				rid.hwndTarget = windowHandle;
				rawInputDevices.EmplaceBack(rid);
				break;
			}
			case DeviceType::GAMEPAD: {
				RAWINPUTDEVICE rid;
				rid.usUsagePage = HID_USAGE_PAGE_GENERIC;
				rid.usUsage = HID_USAGE_GENERIC_JOYSTICK;
				rid.dwFlags = RIDEV_DEVNOTIFY;
				rid.hwndTarget = windowHandle;
				rawInputDevices.EmplaceBack(rid);
				break;
			}
			}

			if (!RegisterRawInputDevices(rawInputDevices.begin(), rawInputDevices.GetLength(), sizeof(RAWINPUTDEVICE))) {
				auto errorCode = GetLastError();
				GTSL_ASSERT(false, "Failed to register devices");
			}
#elif __linux__
#endif
		}
		
		enum class ProgressState {
			NONE,
			INDETERMINATE,
			NORMAL,
			PAUSED,
			ERROR
		};
		void SetProgressState(ProgressState progressState) const {
#if (_WIN64)
			TBPFLAG flags = TBPF_NOPROGRESS;

#undef ERROR

			switch (progressState) {
			case ProgressState::NONE: break;
			case ProgressState::INDETERMINATE: flags = TBPF_INDETERMINATE; break;
			case ProgressState::NORMAL: flags = TBPF_NORMAL; break;
			case ProgressState::PAUSED: flags = TBPF_PAUSED; break;
			case ProgressState::ERROR: flags = TBPF_ERROR; break;
			default: flags = TBPF_NOPROGRESS;
			}

			//static_cast<ITaskbarList3*>(iTaskbarList)->SetProgressState(static_cast<HWND>(windowHandle), flags);
#elif __linux__
#endif
		}
		
		void SetProgressValue(float32 value) const {
			//static_cast<ITaskbarList3*>(iTaskbarList)->SetProgressValue(static_cast<HWND>(windowHandle), value * 1000u, 1000u);
		}
	
	protected:
		WindowSizeState windowSizeState;
		bool focus;

#if __linux__
		xcb_connection_t * connection = nullptr;
		xcb_window_t window;
		xcb_screen_t * screen = nullptr;
		xcb_atom_t wmProtocols;
		xcb_atom_t wmDeleteWin;
		GTSL::Extent2D framebufferExtent;
#endif

#if (_WIN64)
		HWND windowHandle = nullptr;
		uint32 defaultWindowStyle{ 0 };

		//void* iTaskbarList;
		//https://gist.github.com/mmozeiko/b8ccc54037a5eaf35432396feabbe435
		struct xbox_state {
			DWORD packet;
			WORD buttons;
			BYTE left_trigger;
			BYTE right_trigger;
			SHORT left_thumb_x;
			SHORT left_thumb_y;
			SHORT right_thumb_x;
			SHORT right_thumb_y;
		};

		struct xbox_battery {
			BYTE type;
			BYTE level;
		};

		struct xbox_caps {
			BYTE type;
			BYTE subtype;
			WORD flags;

			DWORD buttons;
			BYTE left_trigger;
			BYTE right_trigger;
			SHORT left_thumb_x;
			SHORT left_thumb_y;
			SHORT right_thumb_x;
			SHORT right_thumb_y;

			BYTE low_freq;
			BYTE high_freq;
		};

		struct xbox_dev {
			HANDLE handle;
			CHAR path[MAX_PATH];
		};

		static constexpr GUID xbox_guid = { 0xec87f1e3, 0xc13b, 0x4100, { 0xb5, 0xf7, 0x8b, 0x84, 0xd5, 0x42, 0x60, 0xcb } };

		static constexpr uint8 XBOX_MAX_CONTROLLERS = 4;

		static bool IsDS4OnUSBFromVersionNumber(RID_DEVICE_INFO device_info) {
			// RawInputDataFetcher on Windows does not distinguish devices by bus type.
			// Detect the transport in use by inspecting the version number reported by
			// the device.
			if (device_info.hid.dwVersionNumber == 0x0100/*Expected values when connected over USB*/) {
				return true;
			} if (device_info.hid.dwVersionNumber == 0/*Expected values when connected over BT*/) {
				return false;
			}

			return false;
		}

		static bool isXboxController(char* deviceName) {
			return strstr(deviceName, "IG_");
		}

		static bool isDualshock4(RID_DEVICE_INFO_HID info) {
			constexpr DWORD sonyVendorID = 0x054C;
			constexpr DWORD ds4Gen1ProductID = 0x05C4;
			constexpr DWORD ds4Gen2ProductID = 0x09CC;

			return info.dwVendorId == sonyVendorID && (info.dwProductId == ds4Gen1ProductID || info.dwProductId == ds4Gen2ProductID);
		}

		static void xbox_init() {
			DWORD count = 0;
			HANDLE new_handles[XBOX_MAX_CONTROLLERS]{ nullptr };

			HDEVINFO dev = SetupDiGetClassDevsA(&xbox_guid, nullptr, nullptr, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
			if (dev != INVALID_HANDLE_VALUE) {
				SP_DEVICE_INTERFACE_DATA idata;
				idata.cbSize = sizeof(idata);

				DWORD index = 0;
				while (SetupDiEnumDeviceInterfaces(dev, nullptr, &xbox_guid, index, &idata)) {
					DWORD size;
					SetupDiGetDeviceInterfaceDetailA(dev, &idata, nullptr, 0, &size, nullptr);

					HANDLE hHeap = GetProcessHeap();

					if (const auto detail = static_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA_A>(HeapAlloc(hHeap, 0, size))) {
						detail->cbSize = sizeof(*detail); // yes, sizeof of structure, not allocated memory

						SP_DEVINFO_DATA data;
						data.cbSize = sizeof(data);

						if (SetupDiGetDeviceInterfaceDetailA(dev, &idata, detail, size, &size, &data)) {
							auto controller = xbox_dev();
							xbox_connect(controller, detail->DevicePath);
						}

						HeapFree(hHeap, 0, detail);
					}

					index++;
				}

				SetupDiDestroyDeviceInfoList(dev);
			}
		}

		static bool xbox_connect(xbox_dev& xbox_dev, const char* path) {
			// yes, _stricmp, because SetupDi* functions and WM_DEVICECHANGE message provides different case paths...
			if (xbox_dev.handle != nullptr && _stricmp(xbox_dev.path, path) == 0) {
				return true;
			}			

			HANDLE handle = CreateFileA(path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
			if (handle == INVALID_HANDLE_VALUE) {
				return false;
			}

			if (!xbox_dev.handle) {
				xbox_dev.handle = handle;
				strncpy_s(xbox_dev.path, path, MAX_PATH);
				return true;
			}

			return false;
		}

		static bool xbox_disconnect(xbox_dev& dev, const char* path) {
			if (dev.handle != nullptr && _stricmp(dev.path, path) == 0) {
				CloseHandle(dev.handle);
				dev.handle = nullptr;
				return true;
			}			

			return false;
		}

		static bool xbox_get_caps(xbox_dev xbox_dev, xbox_caps* caps) {
			if (!xbox_dev.handle) { return false; }

			BYTE in[3] = { 0x01, 0x01, 0x00 };
			BYTE out[24];
			DWORD size;
			if (!DeviceIoControl(xbox_dev.handle, 0x8000e004, in, sizeof(in), out, sizeof(out), &size, nullptr) || size != sizeof(out)) {
				// NOTE: could check GetLastError() here, if it is ERROR_DEVICE_NOT_CONNECTED - that means disconnect
				return false;
			}

			caps->type = out[2];
			caps->subtype = out[3];
			caps->flags = 4; // yes, always 4
			caps->buttons = *(WORD*)(out + 4);
			caps->left_trigger = out[6];
			caps->right_trigger = out[7];
			caps->left_thumb_x = *(SHORT*)(out + 8);
			caps->left_thumb_y = *(SHORT*)(out + 10);
			caps->right_thumb_x = *(SHORT*)(out + 12);
			caps->right_thumb_y = *(SHORT*)(out + 14);
			caps->low_freq = out[22];
			caps->high_freq = out[23];
			return true;
		}

		static bool xbox_get_battery(xbox_dev xbox_dev, xbox_battery* bat) {
			if (!xbox_dev.handle) { return false; }

			BYTE in[4] = { 0x02, 0x01, 0x00, 0x00 };
			BYTE out[4];
			DWORD size;
			if (!DeviceIoControl(xbox_dev.handle, 0x8000e018, in, sizeof(in), out, sizeof(out), &size, nullptr) || size != sizeof(out)) {
				// NOTE: could check GetLastError() here, if it is ERROR_DEVICE_NOT_CONNECTED - that means disconnect
				return false;
			}

			bat->type = out[2];
			bat->level = out[3];
			return true;
		}


		static bool xbox_get(xbox_dev xbox_dev, xbox_state* state) {
			if (!xbox_dev.handle) { return false; }

			BYTE in[3] = { 0x01, 0x01, 0x00 };
			BYTE out[29];
			DWORD size;
			if (!DeviceIoControl(xbox_dev.handle, 0x8000e00c, in, sizeof(in), out, sizeof(out), &size, nullptr) || size != sizeof(out)) {
				// NOTE: could check GetLastError() here, if it is ERROR_DEVICE_NOT_CONNECTED - that means disconnect
				return false;
			}

			state->packet = *(DWORD*)(out + 5);
			state->buttons = *(WORD*)(out + 11);
			state->left_trigger = out[13];
			state->right_trigger = out[14];
			state->left_thumb_x = *(SHORT*)(out + 15);
			state->left_thumb_y = *(SHORT*)(out + 17);
			state->right_thumb_x = *(SHORT*)(out + 19);
			state->right_thumb_y = *(SHORT*)(out + 21);
			return true;
		}


		static bool xbox_set(xbox_dev xbox_dev, BYTE low_freq, BYTE high_freq) {
			if (!xbox_dev.handle) { return false; }

			BYTE in[5] = { 0, 0, low_freq, high_freq, 2 };
			if (!DeviceIoControl(xbox_dev.handle, 0x8000a010, in, sizeof(in), nullptr, 0, nullptr, nullptr)) {
				// NOTE: could check GetLastError() here, if it is ERROR_DEVICE_NOT_CONNECTED - that means disconnect
				return false;
			}

			return true;
		}

		static uint64 __stdcall Win32_windowProc(void* hwnd, uint32 uMsg, uint64 wParam, uint64 lParam) {
			auto* windowCallData = reinterpret_cast<WindowsCallData*>(GetWindowLongPtrA(static_cast<HWND>(hwnd), GWLP_USERDATA));
			const HWND winHandle = static_cast<HWND>(hwnd);
			if (!windowCallData) { return DefWindowProcA(winHandle, uMsg, wParam, lParam); }

			switch (uMsg) {
			case WM_CREATE: {
				auto* createStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
				windowCallData = static_cast<WindowsCallData*>(createStruct->lpCreateParams);
				SetWindowLongPtrA(static_cast<HWND>(hwnd), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(windowCallData));
				break;
			}
			case WM_CLOSE: {
				windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::CLOSE, nullptr);
				return 0;
			}
			case WM_LBUTTONDOWN: {
				MouseButtonEventData mouseButtonEventData;
				mouseButtonEventData.State = true;
				mouseButtonEventData.Button = MouseButton::LEFT_BUTTON;
				windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::MOUSE_BUTTON, &mouseButtonEventData);
				return 0;
			}
			case WM_LBUTTONUP: {
				MouseButtonEventData mouseButtonEventData;
				mouseButtonEventData.State = false;
				mouseButtonEventData.Button = MouseButton::LEFT_BUTTON;
				windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::MOUSE_BUTTON, &mouseButtonEventData);
				return 0;
			}
			case WM_RBUTTONDOWN: {
				MouseButtonEventData mouseButtonEventData;
				mouseButtonEventData.State = true;
				mouseButtonEventData.Button = MouseButton::RIGHT_BUTTON;
				windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::MOUSE_BUTTON, &mouseButtonEventData);
				return 0;
			}
			case WM_RBUTTONUP: {
				MouseButtonEventData mouseButtonEventData;
				mouseButtonEventData.State = false;
				mouseButtonEventData.Button = MouseButton::RIGHT_BUTTON;
				windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::MOUSE_BUTTON, &mouseButtonEventData);
				return 0;
			}
			case WM_MBUTTONDOWN: {
				MouseButtonEventData mouseButtonEventData;
				mouseButtonEventData.State = true;
				mouseButtonEventData.Button = MouseButton::MIDDLE_BUTTON;
				windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::MOUSE_BUTTON, &mouseButtonEventData);
				return 0;
			}
			case WM_MBUTTONUP: {
				MouseButtonEventData mouseButtonEventData;
				mouseButtonEventData.State = false;
				mouseButtonEventData.Button = MouseButton::MIDDLE_BUTTON;
				windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::MOUSE_BUTTON, &mouseButtonEventData);
				return 0;
			}
			case WM_KEYDOWN: {
				KeyboardKeyEventData keyboardKeyPressEventData;
				Win32_translateKeys(wParam, lParam, keyboardKeyPressEventData.Key);
				keyboardKeyPressEventData.State = true;
				keyboardKeyPressEventData.IsFirstTime = !((lParam >> 30) & 1);
				windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::KEYBOARD_KEY, &keyboardKeyPressEventData);
				return 0;
			}
			case WM_KEYUP: {
				KeyboardKeyEventData keyboardKeyPressEventData;
				Win32_translateKeys(wParam, lParam, keyboardKeyPressEventData.Key);
				keyboardKeyPressEventData.State = false;
				keyboardKeyPressEventData.IsFirstTime = true;
				windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::KEYBOARD_KEY, &keyboardKeyPressEventData);
				return 0;
			}
			case WM_UNICHAR: {
				CharEventData charEventData = static_cast<uint32>(wParam);
				windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::CHAR, &charEventData);
				return 0;
			}
			case WM_SIZE: {
				WindowSizeEventData clientSize;
				clientSize.Width = LOWORD(lParam);
				clientSize.Height = HIWORD(lParam);

				windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::SIZE, &clientSize);
				return 0;
			}
			case WM_SIZING: {
				RECT rect = *reinterpret_cast<RECT*>(lParam);

				if (!windowCallData->hadResize) {
					WindowSizeEventData clientSize;
					clientSize.Width = static_cast<uint16>(rect.right);
					clientSize.Height = static_cast<uint16>(rect.bottom);

					windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::SIZE, &clientSize);

					windowCallData->hadResize = true;
				}

				return true; //An application should return TRUE if it processes this message.
			}
			case WM_MOVING: {
				RECT rect;
				Extent2D windowSize;
				GetWindowRect(winHandle, &rect);
				windowSize.Width = static_cast<uint16>(rect.right);
				windowSize.Height = static_cast<uint16>(rect.bottom);

				WindowMoveEventData windowMoveEventData;
				windowMoveEventData.X = static_cast<uint16>(reinterpret_cast<LPRECT>(lParam)->left + static_cast<LONG>(windowSize.Width) / 2);
				windowMoveEventData.Y = static_cast<uint16>(reinterpret_cast<LPRECT>(lParam)->top - static_cast<LONG>(windowSize.Height) / 2);

				windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::MOVING, &windowMoveEventData);
				return true; //An application should return TRUE if it processes this message.
			}
			case WM_ACTIVATE: {
				auto hasFocus = static_cast<bool>(LOWORD(wParam));
				FocusEventData focusEventData;
				focusEventData.Focus = hasFocus;
				focusEventData.HadFocus = windowCallData->WindowPointer->focus;
				windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::FOCUS, &focusEventData);
				windowCallData->WindowPointer->focus = hasFocus;
				return 0;
			}
			case WM_SETFOCUS: {
				FocusEventData focusEventData;
				focusEventData.Focus = true;
				focusEventData.HadFocus = windowCallData->WindowPointer->focus;
				windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::FOCUS, &focusEventData);
				windowCallData->WindowPointer->focus = true;
				return 0;
			}
			case WM_KILLFOCUS: {
				FocusEventData focusEventData;
				focusEventData.Focus = false;
				focusEventData.HadFocus = windowCallData->WindowPointer->focus;
				windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::FOCUS, &focusEventData);
				windowCallData->WindowPointer->focus = false;
				return 0;
			}
			case WM_INPUT: {
				PRAWINPUT pRawInput; UINT dataSize = 0; HANDLE hHeap = GetProcessHeap();

				GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, nullptr, &dataSize, sizeof(RAWINPUTHEADER));

				pRawInput = static_cast<PRAWINPUT>(HeapAlloc(hHeap, 0, dataSize));
				if (!pRawInput)
					return 0;

				GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, pRawInput, &dataSize, sizeof(RAWINPUTHEADER));

				switch (pRawInput->header.dwType) {
				case RIM_TYPEMOUSE: { //mouse
					MouseMoveEventData result;
					const int32 x = pRawInput->data.mouse.lLastX, y = pRawInput->data.mouse.lLastY;

					if ((pRawInput->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE) == MOUSE_MOVE_ABSOLUTE) {
						const bool isVirtualDesktop = (pRawInput->data.mouse.usFlags & MOUSE_VIRTUAL_DESKTOP) == MOUSE_VIRTUAL_DESKTOP;

						int width = GetSystemMetrics(isVirtualDesktop ? SM_CXVIRTUALSCREEN : SM_CXSCREEN);
						int height = GetSystemMetrics(isVirtualDesktop ? SM_CYVIRTUALSCREEN : SM_CYSCREEN);

						int absoluteX = static_cast<int>(pRawInput->data.mouse.lLastX / 65535.0f * width);
						int absoluteY = static_cast<int>(pRawInput->data.mouse.lLastY / 65535.0f * height);

						result.X() = static_cast<float32>(absoluteX); result.Y() = static_cast<float32>(absoluteY);
					}

					if ((pRawInput->data.mouse.usFlags & MOUSE_MOVE_RELATIVE) == MOUSE_MOVE_RELATIVE) {
						result.X() = static_cast<float32>(x); result.Y() = static_cast<float32>(-y);
					}

					//set mouse move variable, then at end of window update cycle it will be read and only the last event will be reported
					windowCallData->MouseMove = result;

					// Wheel data needs to be pointer casted to interpret an unsigned short as a short, with no conversion
					// otherwise it'll overflow when going negative.
					if (pRawInput->data.mouse.usButtonFlags & RI_MOUSE_WHEEL) {
						MouseWheelEventData mouseWheelEventData = -static_cast<float32>(*reinterpret_cast<short*>(&pRawInput->data.mouse.usButtonData)) / static_cast<float32>(WHEEL_DELTA);
						windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::MOUSE_WHEEL, &mouseWheelEventData);
					}

					break;
				}
				case RIM_TYPEHID: {
					xbox_state state;

					xbox_get(xbox_dev(), &state);

					WindowGamepad gamepadQuery;

					if (state.left_trigger != gamepadQuery.leftTrigger) {
						const auto value = static_cast<float>(state.left_trigger) / 255.0f;
						gamepadQuery.leftTrigger = state.left_trigger;

						JoystickUpdate result{ Gamepad::Side::LEFT, Gamepad::SourceNames::TRIGGER, { .Linear = value } };
						windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::JOYSTICK_UPDATE, &result);
					}

					if (state.right_trigger != gamepadQuery.rightTrigger) {
						const auto value = static_cast<float>(state.right_trigger) / 255.0f;
						gamepadQuery.rightTrigger = state.right_trigger;

						JoystickUpdate result{ Gamepad::Side::RIGHT, Gamepad::SourceNames::TRIGGER, {.Linear = value } };
						windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::JOYSTICK_UPDATE, &result);
					}

					if (state.left_thumb_x != gamepadQuery.thumbLX || state.left_thumb_y != gamepadQuery.thumbLY) {
						const auto value = Vector2(static_cast<float>(state.left_thumb_x) / 32768.f, static_cast<float>(state.left_thumb_y) / 32768.f);
						gamepadQuery.thumbLX = state.left_thumb_x; gamepadQuery.thumbLY = state.left_thumb_y;

						JoystickUpdate result{ Gamepad::Side::LEFT, Gamepad::SourceNames::THUMB, { .Vector = value } };
						windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::JOYSTICK_UPDATE, &result);
					}

					if (state.right_thumb_x != gamepadQuery.thumbRX || state.right_thumb_y != gamepadQuery.thumbRY) {
						const auto value = Vector2(static_cast<float>(state.right_thumb_x) / 32768.f, static_cast<float>(state.right_thumb_y) / 32768.f);
						gamepadQuery.thumbRX = state.right_thumb_x; gamepadQuery.thumbRY = state.right_thumb_y;

						JoystickUpdate result{ Gamepad::Side::RIGHT, Gamepad::SourceNames::THUMB, { .Vector = value } };
						windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::JOYSTICK_UPDATE, &result);
					}

					if (static_cast<bool>(state.buttons & XINPUT_GAMEPAD_DPAD_UP) != gamepadQuery.dpadUp) {
						const auto value = static_cast<bool>(state.buttons & XINPUT_GAMEPAD_DPAD_UP);
						gamepadQuery.dpadUp = value;

						JoystickUpdate result{ Gamepad::Side::UP, Gamepad::SourceNames::DPAD, { .Action = value } };
						windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::JOYSTICK_UPDATE, &result);
					}

					if (static_cast<bool>(state.buttons & XINPUT_GAMEPAD_DPAD_DOWN) != gamepadQuery.dpadDown) {
						const auto value = static_cast<bool>(state.buttons & XINPUT_GAMEPAD_DPAD_DOWN);
						gamepadQuery.dpadDown = value;

						JoystickUpdate result{ Gamepad::Side::DOWN, Gamepad::SourceNames::DPAD, {.Action = value } };
						windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::JOYSTICK_UPDATE, &result);
					}

					if (static_cast<bool>(state.buttons & XINPUT_GAMEPAD_DPAD_LEFT) != gamepadQuery.dpadLeft) {
						const auto value = static_cast<bool>(state.buttons & XINPUT_GAMEPAD_DPAD_LEFT);
						gamepadQuery.dpadLeft = value;

						JoystickUpdate result{ Gamepad::Side::LEFT, Gamepad::SourceNames::DPAD, {.Action = value } };
						windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::JOYSTICK_UPDATE, &result);
					}

					if (static_cast<bool>(state.buttons & XINPUT_GAMEPAD_DPAD_RIGHT) != gamepadQuery.dpadRight) {
						const auto value = static_cast<bool>(state.buttons & XINPUT_GAMEPAD_DPAD_RIGHT);
						gamepadQuery.dpadRight = value;

						JoystickUpdate result{ Gamepad::Side::RIGHT, Gamepad::SourceNames::DPAD, {.Action = value } };
						windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::JOYSTICK_UPDATE, &result);
					}

					if (static_cast<bool>(state.buttons & XINPUT_GAMEPAD_START) != gamepadQuery.start) {
						const auto value = static_cast<bool>(state.buttons & XINPUT_GAMEPAD_START);
						gamepadQuery.start = value;

						JoystickUpdate result{ Gamepad::Side::RIGHT, Gamepad::SourceNames::MIDDLE_BUTTONS, {.Action = value } };
						windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::JOYSTICK_UPDATE, &result);
					}

					if (static_cast<bool>(state.buttons & XINPUT_GAMEPAD_BACK) != gamepadQuery.back) {
						const auto value = static_cast<bool>(state.buttons & XINPUT_GAMEPAD_BACK);
						gamepadQuery.back = value;

						JoystickUpdate result{ Gamepad::Side::LEFT, Gamepad::SourceNames::MIDDLE_BUTTONS, {.Action = value } };
						windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::JOYSTICK_UPDATE, &result);
					}

					if (static_cast<bool>(state.buttons & XINPUT_GAMEPAD_LEFT_THUMB) != gamepadQuery.leftThumb) {
						const auto value = static_cast<bool>(state.buttons & XINPUT_GAMEPAD_LEFT_THUMB);
						gamepadQuery.leftThumb = value;

						JoystickUpdate result{ Gamepad::Side::LEFT, Gamepad::SourceNames::THUMB_BUTTONS, {.Action = value } };
						windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::JOYSTICK_UPDATE, &result);
					}

					if (static_cast<bool>(state.buttons & XINPUT_GAMEPAD_RIGHT_THUMB) != gamepadQuery.rightThumb) {
						const auto value = static_cast<bool>(state.buttons & XINPUT_GAMEPAD_RIGHT_THUMB);
						gamepadQuery.rightThumb = value;

						JoystickUpdate result{ Gamepad::Side::RIGHT, Gamepad::SourceNames::THUMB_BUTTONS, {.Action = value } };
						windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::JOYSTICK_UPDATE, &result);
					}

					if (static_cast<bool>(state.buttons & XINPUT_GAMEPAD_LEFT_SHOULDER) != gamepadQuery.leftShoulder) {
						const auto value = static_cast<bool>(state.buttons & XINPUT_GAMEPAD_LEFT_SHOULDER);
						gamepadQuery.leftShoulder = value;

						JoystickUpdate result{ Gamepad::Side::LEFT, Gamepad::SourceNames::SHOULDER, {.Action = value } };
						windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::JOYSTICK_UPDATE, &result);
					}

					if (static_cast<bool>(state.buttons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != gamepadQuery.rightShoulder) {
						const auto value = static_cast<bool>(state.buttons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
						gamepadQuery.rightShoulder = value;

						JoystickUpdate result{ Gamepad::Side::RIGHT, Gamepad::SourceNames::SHOULDER, {.Action = value } };
						windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::JOYSTICK_UPDATE, &result);
					}

					if (static_cast<bool>(state.buttons & XINPUT_GAMEPAD_A) != gamepadQuery.a) {
						const auto value = static_cast<bool>(state.buttons & XINPUT_GAMEPAD_A);
						gamepadQuery.a = value;

						JoystickUpdate result{ Gamepad::Side::DOWN, Gamepad::SourceNames::FACE_BUTTONS, {.Action = value } };
						windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::JOYSTICK_UPDATE, &result);
					}

					if (static_cast<bool>(state.buttons & XINPUT_GAMEPAD_B) != gamepadQuery.b) {
						const auto value = static_cast<bool>(state.buttons & XINPUT_GAMEPAD_B);
						gamepadQuery.b = value;

						JoystickUpdate result{ Gamepad::Side::RIGHT, Gamepad::SourceNames::FACE_BUTTONS, {.Action = value } };
						windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::JOYSTICK_UPDATE, &result);
					}

					if (static_cast<bool>(state.buttons & XINPUT_GAMEPAD_X) != gamepadQuery.x) {
						const auto value = static_cast<bool>(state.buttons & XINPUT_GAMEPAD_X);
						gamepadQuery.x = value;

						JoystickUpdate result{ Gamepad::Side::LEFT, Gamepad::SourceNames::FACE_BUTTONS, {.Action = value } };
						windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::JOYSTICK_UPDATE, &result);
					}

					if (static_cast<bool>(state.buttons & XINPUT_GAMEPAD_Y) != gamepadQuery.y) {
						const auto value = static_cast<bool>(state.buttons & XINPUT_GAMEPAD_Y);
						gamepadQuery.y = value;

						JoystickUpdate result{ Gamepad::Side::UP, Gamepad::SourceNames::FACE_BUTTONS, {.Action = value } };
						windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::JOYSTICK_UPDATE, &result);
					}
				}
				}

				HeapFree(hHeap, 0, pRawInput);

				return 0;
			}
			case WM_DEVICECHANGE: {
				const DEV_BROADCAST_HDR* hdr = reinterpret_cast<const DEV_BROADCAST_HDR*>(lParam);

				if (hdr->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE) {
					const DEV_BROADCAST_DEVICEINTERFACE_A* dif = reinterpret_cast<const DEV_BROADCAST_DEVICEINTERFACE_A*>(hdr);

					if (wParam == DBT_DEVICEARRIVAL) {
						xbox_dev dev;
						if (!xbox_connect(dev, dif->dbcc_name)) { return 0; }

						xbox_caps caps;
						xbox_battery bat;
						if (xbox_get_caps(dev, &caps) == 0 && xbox_get_battery(dev, &bat) == 0) {
						}
					} else if (wParam == DBT_DEVICEREMOVECOMPLETE) {
						xbox_dev devs[32];

						uint32 i = 0;

						while(!xbox_disconnect(devs[i], dif->dbcc_name)) {
							++i;
						}
					}
				}
				
				return 0;
			}
			case WM_INPUT_DEVICE_CHANGE: {
				auto rawInputDevice = reinterpret_cast<HANDLE>(lParam);

				//RID_DEVICE_INFO_HID device_info;
				//isDualshock4(device_info);// isXboxController(device_info);

				DeviceChangeData result;
				windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::DEVICE_CHANGE, &result);

				return 0;
			}
			case WM_DPICHANGED: {
				auto dpi = HIWORD(wParam);

				PPIChangeData result;
				result.PPI = dpi;
				windowCallData->FunctionToCall(windowCallData->UserData, WindowEvents::PPI_CHANGE, &result);

				return 0;
			}
			}

			return DefWindowProcA(winHandle, uMsg, wParam, lParam);
		}

		void SendKeyboardInput(KeyboardKeys key) {
			INPUT input;
			input.type = 1;
			input.ki.time = 0;
			input.ki.wScan = 0;

			switch (key) {
			case KeyboardKeys::Q: break; case KeyboardKeys::W: break;
			case KeyboardKeys::E: break; case KeyboardKeys::R: break;
			case KeyboardKeys::T: break; case KeyboardKeys::Y: break;
			case KeyboardKeys::U: break; case KeyboardKeys::I: break;
			case KeyboardKeys::O: break; case KeyboardKeys::P: break;
			case KeyboardKeys::A: input.ki.wVk = 0x41; break; case KeyboardKeys::S: break;
			case KeyboardKeys::D: break; case KeyboardKeys::F: break;
			case KeyboardKeys::G: break; case KeyboardKeys::H: break;
			case KeyboardKeys::J: break; case KeyboardKeys::K: break;
			case KeyboardKeys::L: break; case KeyboardKeys::Z: break;
			case KeyboardKeys::X: break; case KeyboardKeys::C: break;
			case KeyboardKeys::V: break; case KeyboardKeys::B: break;
			case KeyboardKeys::N: break; case KeyboardKeys::M: break;
			case KeyboardKeys::Keyboard0: break; case KeyboardKeys::Keyboard1: break;
			case KeyboardKeys::Keyboard2: break; case KeyboardKeys::Keyboard3: break;
			case KeyboardKeys::Keyboard4: break; case KeyboardKeys::Keyboard5: break;
			case KeyboardKeys::Keyboard6: break; case KeyboardKeys::Keyboard7: break;
			case KeyboardKeys::Keyboard8: break; case KeyboardKeys::Keyboard9: break;
			case KeyboardKeys::Backspace: break;
			case KeyboardKeys::Enter: break;
			case KeyboardKeys::Supr: break;
			case KeyboardKeys::Tab: break;
			case KeyboardKeys::CapsLock: break;
			case KeyboardKeys::Esc: break;
			case KeyboardKeys::RShift: break;
			case KeyboardKeys::LShift: break;
			case KeyboardKeys::RControl: break;
			case KeyboardKeys::LControl: break;
			case KeyboardKeys::Alt: break;
			case KeyboardKeys::AltGr: break;
			case KeyboardKeys::UpArrow: break;
			case KeyboardKeys::RightArrow: break;
			case KeyboardKeys::DownArrow: break;
			case KeyboardKeys::LeftArrow: break;
			case KeyboardKeys::SpaceBar: break;
			case KeyboardKeys::Numpad0: break;
			case KeyboardKeys::Numpad1: break;
			case KeyboardKeys::Numpad2: break;
			case KeyboardKeys::Numpad3: break;
			case KeyboardKeys::Numpad4: break;
			case KeyboardKeys::Numpad5: break;
			case KeyboardKeys::Numpad6: break;
			case KeyboardKeys::Numpad7: break;
			case KeyboardKeys::Numpad8: break;
			case KeyboardKeys::Numpad9: break;
			case KeyboardKeys::F1: break;
			case KeyboardKeys::F2: break;
			case KeyboardKeys::F3: break;
			case KeyboardKeys::F4: break;
			case KeyboardKeys::F5: break;
			case KeyboardKeys::F6: break;
			case KeyboardKeys::F7: break;
			case KeyboardKeys::F8: break;
			case KeyboardKeys::F9: break;
			case KeyboardKeys::F10: break;
			case KeyboardKeys::F11: break;
			case KeyboardKeys::F12: break;
			}

			SendInput(1, &input, sizeof(INPUT));
		}

		static void Win32_calculateMousePos(uint32 x, uint32 y, Extent2D clientSize, Vector2& mousePos) {
			const auto halfX = static_cast<float>(clientSize.Width) * 0.5f;
			const auto halfY = static_cast<float>(clientSize.Height) * 0.5f;
			mousePos.X() = (x - halfX) / halfX; mousePos.Y() = (halfY - y) / halfY;
		}
		
		static void Win32_translateKeys(uint64 win32Key, uint64 context, KeyboardKeys& key) {
			switch (win32Key) {
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

		struct WindowGamepad {
			byte leftTrigger = 0, rightTrigger = 0;
			int16 thumbLX = 0, thumbLY = 0, thumbRX = 0, thumbRY = 0;
			bool dpadUp : 1 = false, dpadRight : 1 = false, dpadDown : 1 = false, dpadLeft : 1 = false;
			bool start : 1 = false, back : 1 = false;
			bool leftThumb : 1 = false, rightThumb : 1 = false;
			bool leftShoulder : 1 = false, rightShoulder : 1 = false;
			bool a : 1 = false, b : 1 = false, x : 1 = false, y : 1 = false;
		};
	};
}
