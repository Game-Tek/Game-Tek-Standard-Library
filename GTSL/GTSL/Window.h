#pragma once

#include "Extent.h"

#include "Delegate.hpp"
#include "Input.h"
#include "String.hpp"
#include "Math/Vector2.h"

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

		enum class WindowStyle
		{
			TITLE_BAR = 0,
		};

		enum class WindowSizeState : uint8
		{
			MINIMIZED, MAXIMIZED, FULLSCREEN
		};
		
	protected:
		Extent2D windowSize;
		Extent2D clientSize;
		WindowSizeState windowSizeState;

		void* windowHandle = nullptr;
		uint32 defaultWindowStyle{ 0 };

#if (_WIN32)
		static uint64 __stdcall Win32_windowProc(void* hwnd, uint32 uMsg, uint64 wParam, uint64 lParam);
		void Win32_calculateMousePos(uint32 x, uint32 y, Vector2& mousePos) const;
		static void Win32_translateKeys(uint64 win32Key, uint64 context, KeyboardKeys& key);
#endif
		
		Delegate<void()> onCloseDelegate;
		Delegate<void(const Extent2D&)> onResizeDelegate;
		Delegate<void(MouseButton, ButtonState)> onMouseButtonClick;
		/**
		 * \brief Delegate called when mouse moves, the first two floats are the X;Y in the -1 <-> 1 range, and the other two floats are delta position in the same range in respect to the last update to the screen.
		 */
		Delegate<void(Vector2)> onMouseMove;
		Delegate<void(float)> onMouseWheelMove;
		Delegate<void(KeyboardKeys, ButtonState)> onKeyEvent;
		Delegate<void(uint32)> onCharEvent;
		Delegate<void(uint16, uint16)> onWindowMove;
	public:
		Window() = default;
		struct WindowCreateInfo
		{
			String Name;
			Extent2D Extent;
			Window* ParentWindow = nullptr;
			class Application* Application = nullptr;
		};
		explicit Window(const WindowCreateInfo& windowCreateInfo);
		~Window();
		
		void SetTitle(const char* title);
		void Notify();

		struct WindowIconInfo
		{
			byte* Data = nullptr;
			Extent2D Extent;
		};
		void SetIcon(const WindowIconInfo& windowIconInfo);

		void GetFramebufferExtent(Extent2D& extent) const;
		void GetWindowExtent(Extent2D& windowExtent) const { windowExtent = windowSize; }

		static void GetAspectRatio(const Extent2D& extent, float& aspectRatio) { aspectRatio = static_cast<float>(extent.Width) / static_cast<float>(extent.Height); }

		void SetOnCloseDelegate(const decltype(onCloseDelegate)& delegate) { onCloseDelegate = delegate; }
		/**
		 * \brief Sets the delegate to call when the mouse on this window is moved.
		 * The first two floats are the X;Y in the -1 <-> 1 range, and the other two floats are delta position in the same range in respect to the last update to the window.
		 * TOP = 1, BOTTOM = -1, LEFT = -1, RIGHT = 1
		 * \param delegate Delegate to bind.
		 */
		void SetOnMouseMoveDelegate(const decltype(onMouseMove)& delegate) { onMouseMove = delegate; }
		void SetOnMouseWheelMoveDelegate(const decltype(onMouseWheelMove)& delegate) { onMouseWheelMove = delegate; }
		void SetOnResizeDelegate(const decltype(onResizeDelegate)& delegate) { onResizeDelegate = delegate; }
		void SetOnMouseButtonClickDelegate(const decltype(onMouseButtonClick)& delegate) { onMouseButtonClick = delegate; }
		void SetOnWindowResizeDelegate(const decltype(onResizeDelegate)& delegate) { onResizeDelegate = delegate; }
		void SetOnCharEventDelegate(const decltype(onCharEvent)& delegate) { onCharEvent = delegate; }
		void SetOnKeyEvent(const decltype(onKeyEvent)& delegate) { onKeyEvent = delegate; }

		struct WindowState
		{
			WindowSizeState NewWindowSizeState;
			uint32 RefreshRate{ 0 };
			Extent2D NewResolution;
			uint8 NewBitsPerPixel = 8;
		};
		void SetState(const WindowState& windowState);

		struct Win32NativeHandles
		{
			void* HWND{ nullptr };
		};
		void GetNativeHandles(void* nativeHandlesStruct) const;

		void ShowWindow();
		void HideWindow();
	};
}
