#pragma once

#include "Extent.h"

#include "Delegate.hpp"
#include "Flags.h"
#include "Range.h"
#include "Math/Vectors.h"

#undef ERROR

namespace GTSL
{
	class Window
	{
	public:
		enum class MouseButton : uint8
		{
			LEFT_BUTTON, RIGHT_BUTTON, MIDDLE_BUTTON
		};

		enum class KeyboardKeys : uint8
		{
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

		enum class WindowType : uint8
		{
			OS_WINDOW, NO_ELEMENTS
		};

		enum class WindowSizeState : uint8
		{
			MINIMIZED, MAXIMIZED, FULLSCREEN
		};
		
		Window() = default;

		enum class WindowEvents
		{
			CLOSE, KEYBOARD_KEY, CHAR, SIZE, MOVING, MOUSE_MOVE, MOUSE_WHEEL,
			MOUSE_BUTTON, FOCUS
		};
		
		struct WindowCreateInfo
		{
			Range<const UTF8*> Name;
			Extent2D Extent;
			Window* ParentWindow = nullptr;
			class Application* Application = nullptr;
			WindowType Type = WindowType::OS_WINDOW;
			void* UserData;
			Delegate<void(void*, WindowEvents, void*)> Function;
		};
		void BindToOS(const WindowCreateInfo & windowCreateInfo);
		~Window();

		void Update(void* userData, Delegate<void(void*, WindowEvents, void*)>);
		
		void SetTitle(const char* title);
		void Notify();

		struct WindowIconInfo
		{
			byte* Data = nullptr;
			Extent2D Extent;
		};
		void SetIcon(const WindowIconInfo & windowIconInfo);

		Extent2D GetFramebufferExtent() const;
		Extent2D GetWindowExtent() const;

		void SetMousePosition(Extent2D position);
		void LimitMousePosition(Extent2D range);

		void ShowMouse(bool show);
		
		static void GetAspectRatio(const Extent2D & extent, float& aspectRatio) { aspectRatio = static_cast<float>(extent.Width) / static_cast<float>(extent.Height); }

		struct WindowState
		{
			WindowSizeState NewWindowSizeState;
			uint32 RefreshRate{ 0 };
			Extent2D NewResolution;
			uint8 NewBitsPerPixel = 8;
		};
		void SetState(const WindowState & windowState);

		struct Win32NativeHandles
		{
			void* HWND{ nullptr };
		};
		void GetNativeHandles(void* nativeHandlesStruct) const;

		void ShowWindow();
		void HideWindow();

		enum class DeviceType : uint8 { MOUSE, GAMEPAD };
		
		void AddDevice(const DeviceType deviceType);
		
		enum class ProgressState
		{
			NONE,
			INDETERMINATE,
			NORMAL,
			PAUSED,
			ERROR
		};
		void SetProgressState(ProgressState progressState) const;
		void SetProgressValue(float32 value) const;

		struct MouseButtonEventData
		{
			MouseButton Button; bool State;
		};
		struct KeyboardKeyEventData
		{
			KeyboardKeys Key; bool State; bool IsFirstTime;
		};
		using CharEventData = uint32;
		struct WindowMoveEventData
		{
			uint16 X, Y;
		};

		/**
		 * \brief Delegate called when mouse moves, the first two floats are the X;Y in the -1 <-> 1 range, and the other two floats are delta position in the same range in respect to the last update to the screen.
		 */
		using MouseMoveEventData = Vector2;
		using WindowSizeEventData = Extent2D;
		using MouseWheelEventData = float32;
		
		struct FocusEventData
		{
			bool Focus, HadFocus;
		};
	
	protected:
		WindowSizeState windowSizeState;

		void* windowHandle = nullptr;
		uint32 defaultWindowStyle{ 0 };

		bool focus : 1 = false;
		//void* iTaskbarList;

#if (_WIN32)
		static uint64 __stdcall Win32_windowProc(void* hwnd, uint32 uMsg, uint64 wParam, uint64 lParam);
		static void Win32_calculateMousePos(uint32 x, uint32 y, Extent2D windowExtent, Vector2& mousePos);
		static void Win32_translateKeys(uint64 win32Key, uint64 context, KeyboardKeys& key);
#endif

		struct WindowsCallData
		{
			void* UserData;
			Delegate<void(void*, WindowEvents, void*)> FunctionToCall;
			Window* WindowPointer;
			bool hadResize = false;
		};

		friend class Application;
	};
}
