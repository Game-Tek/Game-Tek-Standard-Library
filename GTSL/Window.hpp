#pragma once

#include <iostream>
#include "Delegates.hpp"
#include "Math/Vectors.hpp"
#include "Math/Extent.hpp"


#include <GLFW/glfw3.h>

#if _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#elif __linux__
#define GLFW_EXPOSE_NATIVE_X11
#endif
#include <GLFW/glfw3native.h>

namespace GTSL
{
	enum class Keys
	{
        Unknown = -1,
        Space = 32,
        Apostrophe = 39,
        Comma = 44,
        Minus = 45,
        Period = 46,
        Slash = 47,
        D0 = 48,
        D1 = 49,
        D2 = 50,
        D3 = 51,
        D4 = 52,
        D5 = 53,
        D6 = 54,
        D7 = 55,
        D8 = 56,
        D9 = 57,
        Semicolon = 59,
        Equal = 61,
        A = 65,
        B = 66,
        C = 67,
        D = 68,
        E = 69,
        F = 70,
        G = 71,
        H = 72,
        I = 73,
        J = 74,
        K = 75,
        L = 76,
        M = 77,
        N = 78,
        O = 79,
        P = 80,
        Q = 81,
        R = 82,
        S = 83,
        T = 84,
        U = 85,
        V = 86,
        W = 87,
        X = 88,
        Y = 89,
        Z = 90,
        LeftBracket = 91,
        Backslash = 92,
        RightBracket = 93,
        GraveAccent = 96,
        World1 = 161,
        World2 = 162,
        Escape = 256,
        Enter = 257,
        Tab = 258,
        Backspace = 259,
        Insert = 260,
        Delete = 261,
        Right = 262,
        Left = 263,
        Down = 264,
        Up = 265,
        PageUp = 266,
        PageDown = 267,
        Home = 268,
        End = 269,
        CapsLock = 280,
        ScrollLock = 281,
        NumLock = 282,
        PrintScreen = 283,
        Pause = 284,
        F1 = 290,
        F2 = 291,
        F3 = 292,
        F4 = 293,
        F5 = 294,
        F6 = 295,
        F7 = 296,
        F8 = 297,
        F9 = 298,
        F10 = 299,
        F11 = 300,
        F12 = 301,
        F13 = 302,
        F14 = 303,
        F15 = 304,
        F16 = 305,
        F17 = 306,
        F18 = 307,
        F19 = 308,
        F20 = 309,
        F21 = 310,
        F22 = 311,
        F23 = 312,
        F24 = 313,
        F25 = 314,
        Kp0 = 320,
        Kp1 = 321,
        Kp2 = 322,
        Kp3 = 323,
        Kp4 = 324,
        Kp5 = 325,
        Kp6 = 326,
        Kp7 = 327,
        Kp8 = 328,
        Kp9 = 329,
        KpDecimal = 330,
        KpDivide = 331,
        KpMultiply = 332,
        KpSubtract = 333,
        KpAdd = 334,
        KpEnter = 335,
        KpEqual = 336,
        LeftShift = 340,
        LeftControl = 341,
        LeftAlt = 342,
        LeftSuper = 343,
        RightShift = 344,
        RightControl = 345,
        RightAlt = 346,
        RightSuper = 347,
        Menu = 348
	};

    enum class MouseButton : uint8
    {
	    LeftButton,
        RightButton,
        MiddleButton,
        Extra1,
        Extra2,
        Extra3,
        Extra4,
        Extra5,
    };

    // TODO: Gamepad


    enum class WindowState : uint8
    {
        None,
	    Fullscreen,
        Minimized,
        Maximized
    };

    enum class WindowEvents : uint8
    {
	    Close,
        Keyboard,
        Char,
        Resize,
        WindowMoving,
        MouseButton,
        MouseMove,
        Scroll
    };

    struct KeyboardEvent
    {
        Keys key;
        bool state = false;
    };

    struct MouseButtonEvent
    {
        MouseButton button;
        bool state = false;
    };

    struct MouseMoveEvent
    {
        Vector2 position;
        Vector2 delta;
    };

    struct WindowMoveEvent
    {
        uint16 X, Y;
    };

    typedef Extent2D WindowResizeEvent;
    typedef float MouseWheelEvent;
    typedef uint32 CharEventData;

    class Window
    {
    public:
        DECLARE_EVENT(OnWindowEvent, Window, Window*, WindowEvents, void*);
        OnWindowEvent WindowDelegate;

        /*struct WindowsCall
        {
            void* UserData;
            OnWindowEvent CallBack;
            Window* WindowPointer;
            bool hadResize = false;
            MouseMoveEvent MouseEvent;
        };*/

        Window(const char* title,const Extent2D& size,WindowState state = WindowState::None, GLFWwindow* share = nullptr)
	        : m_window(glfwCreateWindow(size.Width,size.Height,title,state == WindowState::Fullscreen ? glfwGetPrimaryMonitor() : nullptr, share)),
				m_windowSize(size)
        {
            int x, y;
            glfwGetFramebufferSize(m_window, &x, &y);
            m_framebufferSize = { static_cast<unsigned short>(x),static_cast<unsigned short>(y) };
            SetupCallbacks();
        }

        ~Window()
        {
            glfwDestroyWindow(m_window);
        }

        void SetTitle(const char* title)
        {
            glfwSetWindowTitle(m_window, title);
        }

        void SetFullScreen(bool fullscreen)
        {
	        if(fullscreen)
	        {
                const auto mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
                glfwSetWindowMonitor(m_window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
	        }
            else
            {
                glfwSetWindowMonitor(m_window, nullptr, 0, 0, m_windowSize.Width, m_windowSize.Height, GLFW_DONT_CARE);
            }
        }

        void SetWindowState(WindowState state)
        {
	        switch(state)
	        {
	        case WindowState::Fullscreen:
                SetFullScreen(true);
                break;
            case WindowState::Minimized:
                glfwSetWindowAttrib(m_window, GLFW_MAXIMIZED, GLFW_FALSE);
                break;
            case WindowState::Maximized:
                glfwSetWindowAttrib(m_window, GLFW_MAXIMIZED, GLFW_TRUE);
                break;
	        case WindowState::None:
                break;
	        }
        }

        Extent2D GetFramebufferExtent() const { return m_framebufferSize; }
        Extent2D GetWindowExtent() const { return m_windowSize; }

        void SetWindowVisibility(bool visible)
        {
            glfwSetWindowAttrib(m_window, GLFW_VISIBLE, visible ? GLFW_TRUE : GLFW_FALSE);
        }

        void SetMonitor(GLFWmonitor* monitor)
        {
            glfwSetWindowMonitor(m_window, monitor, 0, 0, m_windowSize.Width, m_windowSize.Height,GLFW_DONT_CARE);
        }

        void StartTextInput()
        {
            m_textInput = true;
        }

        void EndTextInput() { m_textInput = false; }

    	void* GetNativePointer()
        {
#if _WIN32
            return glfwGetWin32Window(m_window);
#elif __linux__
            return glfwGetX11Display(m_window);
#endif
        }

        bool ShouldWindowClose()
        {
            return glfwWindowShouldClose(m_window);
        }

        void ProcessEvents() { glfwPollEvents(); }

    private:
        bool m_textInput;
        GLFWwindow* m_window;
        Extent2D m_windowSize;
        Extent2D m_framebufferSize;
        Vector2 m_mousePosition,m_lastMousePosition;

        void SetupCallbacks()
        {
            glfwSetWindowUserPointer(m_window, this);

            glfwSetWindowPosCallback(m_window, [](GLFWwindow* win, int x, int y)
            {
                auto window = static_cast<Window*>(glfwGetWindowUserPointer(win));

                WindowMoveEvent e = { static_cast<uint16>(x),static_cast<uint16>(y) };
                window->WindowDelegate.Broadcast(window, WindowEvents::WindowMoving, &e);
            });

            glfwSetWindowSizeCallback(m_window, [](GLFWwindow* win, int x, int y)
            {
                auto window = static_cast<Window*>(glfwGetWindowUserPointer(win));

                WindowResizeEvent e = { static_cast<uint16>(x),static_cast<uint16>(y) };
                window->WindowDelegate.Broadcast(window, WindowEvents::Resize, &e);
            });

            glfwSetWindowCloseCallback(m_window, [](GLFWwindow* win)
            {
            	auto window = static_cast<Window*>(glfwGetWindowUserPointer(win));

                window->WindowDelegate.Broadcast(window, WindowEvents::Close, nullptr);
            });

            glfwSetKeyCallback(m_window, [](GLFWwindow* win, int key, int scancode, int action, int modes)
            {
	            auto window = static_cast<Window*>(glfwGetWindowUserPointer(win));

                KeyboardEvent e;
                e.key = static_cast<Keys>(key);
                // TODO: Input Repeating
                e.state = action == GLFW_PRESS;
                window->WindowDelegate.Broadcast(window, WindowEvents::Keyboard, &e);
            });

            glfwSetCharCallback(m_window, [](GLFWwindow* win, unsigned int c)
            {
            	auto window = static_cast<Window*>(glfwGetWindowUserPointer(win));

                if(window->m_textInput)
                {
                    CharEventData e = c;
                    window->WindowDelegate.Broadcast(window, WindowEvents::Char, &e);
                }
            });

            glfwSetMouseButtonCallback(m_window, [](GLFWwindow* win,int button, int action, int mods)
            {
                auto window = static_cast<Window*>(glfwGetWindowUserPointer(win));

                MouseButtonEvent e;
                e.button = static_cast<MouseButton>(button);
                e.state = action == GLFW_PRESS;
                window->WindowDelegate.Broadcast(window, WindowEvents::MouseButton, &e);
            });

            glfwSetCursorPosCallback(m_window, [](GLFWwindow* win, double x, double y)
            {
                auto window = static_cast<Window*>(glfwGetWindowUserPointer(win));

                MouseMoveEvent e;
                window->m_lastMousePosition = window->m_mousePosition;
                window->m_mousePosition = { static_cast<float>(x),static_cast<float>(y) };
                e.position = window->m_mousePosition;
                e.delta = window->m_lastMousePosition - window->m_mousePosition;
                window->WindowDelegate.Broadcast(window, WindowEvents::MouseMove, &e);
            });

            glfwSetScrollCallback(m_window, [](GLFWwindow* win, double x, double y)
            {
                auto window = static_cast<Window*>(glfwGetWindowUserPointer(win));

                MouseWheelEvent e = y;
                window->WindowDelegate.Broadcast(window, WindowEvents::Scroll, &e);
            });
        }
    };
}