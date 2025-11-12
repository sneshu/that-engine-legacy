//
// File: WindowsWindow.cpp
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#include "Core/PCH.hpp"
#include "Platform/Windows/WindowsWindow.hpp"
#include "Core/Event/WindowEvent.hpp"
#include "Core/Event/MouseEvent.hpp"
#include "Core/Event/KeyboardEvent.hpp"

namespace ThatEngine
{
    WindowsWindow::WindowsWindow(const WindowProperties& properties)
    {
        if (!WindowsWindow::Init(properties))
        {
            ShowError("Window Initialization");
        }
    }

    WindowsWindow::~WindowsWindow()
    {
        if (!m_Window) return;

        DestroyWindow(m_Window);
    }

    void WindowsWindow::UpdateSizeData()
    {
        RECT innerRect;
        GetClientRect(m_Window, &innerRect);

        LONG innerWidth = innerRect.right - innerRect.left;
        LONG innerHeight = innerRect.bottom - innerRect.top;

        m_Data.InnerWidth = static_cast<uint32_t>(innerWidth);
        m_Data.InnerHeight = static_cast<uint32_t>(innerHeight);
        
        // Set outer size
        RECT outerRect = { 0, 0, innerWidth, innerHeight };
        DWORD style = GetWindowLong(m_Window, GWL_STYLE);
        DWORD exStyle = GetWindowLong(m_Window, GWL_EXSTYLE);
        AdjustWindowRectEx(&outerRect, style, FALSE, exStyle);
        m_Data.OuterWidth = static_cast<uint32_t>(outerRect.right - outerRect.left);
        m_Data.OuterHeight = static_cast<uint32_t>(outerRect.bottom - outerRect.top); 
        
        // Set clip area rect
        RECT windowRect;
        GetWindowRect(m_Window, &windowRect);
        m_Data.PositionX = static_cast<uint32_t>(windowRect.left);
        m_Data.PositionY = static_cast<uint32_t>(windowRect.top);
        
        POINT topLeftPoint = { 0, 0 };
        ClientToScreen(m_Window, &topLeftPoint);
        m_Data.ClipArea.left = topLeftPoint.x;
        m_Data.ClipArea.top = topLeftPoint.y;
        m_Data.ClipArea.right = topLeftPoint.x + innerWidth;
        m_Data.ClipArea.bottom = topLeftPoint.y + innerHeight;

        // Set window minimized state
        WINDOWPLACEMENT placement;
        GetWindowPlacement(m_Window, &placement);
        m_Data.IsMinimized = (placement.showCmd == SW_MINIMIZE) || (m_Data.InnerWidth == 0) || (m_Data.InnerHeight == 0);
        m_Data.IsFocused = (GetForegroundWindow() == m_Window);
    }

    void WindowsWindow::Update()
    {
        MSG msg;

        while (PeekMessageA(&msg, m_Window, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }

        UpdateMouseDelta();
    }

    // Sync mouse delta event with update loop
    void WindowsWindow::UpdateMouseDelta()
    {
        if (m_Data.AccumulatedMouseDeltaX == 0 && m_Data.AccumulatedMouseDeltaY == 0) return;

        RawMouseMoveEvent rawMouseMove(m_Data.AccumulatedMouseDeltaX, m_Data.AccumulatedMouseDeltaY);
        m_Data.InputEventCallback(rawMouseMove);

        m_Data.AccumulatedMouseDeltaX = 0;
        m_Data.AccumulatedMouseDeltaY = 0;
    }

    void WindowsWindow::LockCursor()
    {
        while(ShowCursor(FALSE) >= 0);
        ClipCursor(&m_Data.ClipArea);
        m_Data.IsCursorLocked = true;
    }

    void WindowsWindow::UnlockCursor()
    {
        while(ShowCursor(TRUE) < 0);
        ClipCursor(0);
        m_Data.IsCursorLocked = false;
    }

    void WindowsWindow::ShowError(const char* errorMessage) const
    {
        THAT_CORE_ERROR(errorMessage);
        MessageBoxA(0, errorMessage, "Error", MB_ICONEXCLAMATION | MB_OK);
    }

    bool WindowsWindow::Init(const WindowProperties& properties)
    {
        HINSTANCE instance = GetModuleHandleA(0);

        THAT_CORE_INFO("Window: Initialization {{ Title: \"{}\", Width: {}, Height: {}, Focused: {} }}", properties.Title, properties.InnerWidth, properties.InnerHeight, properties.IsFocused);

        m_Data = {};
        m_Data.Title = properties.Title;
        m_Data.IsCursorLocked = properties.IsFocused;
        m_Data.AccumulatedMouseDeltaX = 0;
        m_Data.AccumulatedMouseDeltaY = 0;

        WNDCLASS windowClass = {};
        windowClass.lpfnWndProc = WindowsWindow::EventCallback;
        windowClass.hInstance = instance;
        windowClass.lpszClassName = "ThatEngine_Window_Class";

        if (!RegisterClassA(&windowClass))
        {
            ShowError("Window: Error during class registration!");
            return false;
        }

        // Recaculate window size to include window decorations
        DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME;
        RECT outerRect = { 0, 0, static_cast<LONG>(properties.InnerWidth), static_cast<LONG>(properties.InnerHeight) };
        AdjustWindowRect(&outerRect, style, FALSE);

        m_Window = CreateWindowExA(
            WS_EX_APPWINDOW,
            windowClass.lpszClassName,
            0,
            style, 
            CW_USEDEFAULT, 
            CW_USEDEFAULT,
            outerRect.right - outerRect.left,
            outerRect.bottom - outerRect.top,
            0, 
            0,
            instance,
            this
        );

        if (!m_Window)
        {
            ShowError("Window: Error during window creation!");
            return false;
        }

        // Set title
        SetWindowTextA(m_Window, m_Data.Title.c_str());

        // Set icon
        HICON engineIconBig = (HICON)LoadImage(NULL, "Assets/Icons/T_Voxadion_32x32.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_SHARED);
        HICON engineIconSmall = (HICON)LoadImage(NULL, "Assets/Icons/T_Voxadion_16x16.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_SHARED);
        SendMessage(m_Window, WM_SETICON, ICON_BIG, (LPARAM)engineIconBig);
        SendMessage(m_Window, WM_SETICON, ICON_SMALL, (LPARAM)engineIconSmall);

        // Set cursor
        HCURSOR arrowCursor = LoadCursorA(0, IDC_ARROW);
        SetCursor(arrowCursor);

        // Register input devices
        RAWINPUTDEVICE rawInput;
        rawInput.usUsagePage = 1;
        rawInput.usUsage = 2;
        rawInput.dwFlags = 0;
        rawInput.hwndTarget = m_Window;

        if (!RegisterRawInputDevices(&rawInput, 1, sizeof(rawInput)))
        {
            ShowError("Window: Error during input devices registration!");
            return false;
        }

        // Display window
        ShowWindow(m_Window, properties.IsFocused ? SW_SHOW : SW_SHOWNOACTIVATE);

        // Update inner and outer size data
        UpdateSizeData();

        // Lock cursor
        if (m_Data.IsFocused)
        {
            LockCursor();
        }
        return true;
    }

    LRESULT CALLBACK WindowsWindow::EventCallback(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (msg == WM_NCCREATE)
        {
            CREATESTRUCTA* create = reinterpret_cast<CREATESTRUCTA*>(lParam);
            SetWindowLongPtr(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(create->lpCreateParams));
            return true;
        }

        WindowsWindow* windowRef = reinterpret_cast<WindowsWindow*>(GetWindowLongPtr(window, GWLP_USERDATA));
        
        if (!windowRef || !windowRef->m_Data.WindowEventCallback)
        {
            return DefWindowProcA(window, msg, wParam, lParam);
        }

        uint32_t x = LOWORD(lParam);
        uint32_t y = HIWORD(lParam);

        switch(msg)
        {
            case WM_CLOSE:
            {
                WindowCloseEvent windowClose;
                windowRef->m_Data.WindowEventCallback(windowClose);
                return 0;
            }

            case WM_MOVE:
            {
                windowRef->UpdateSizeData();      

                WindowMoveEvent windowMove(x, y);
                windowRef->m_Data.WindowEventCallback(windowMove);
                break;
            }

            case WM_SIZE:
            {
                windowRef->UpdateSizeData();          

                WindowResizeEvent windowResize(x, y);
                windowRef->m_Data.WindowEventCallback(windowResize);
                break;
            }

            case WM_SETFOCUS:
            {
                WindowFocusGainedEvent windowFocusGained;
                windowRef->m_Data.IsFocused = true;
                windowRef->m_Data.WindowEventCallback(windowFocusGained);
                break;
            }

            case WM_KILLFOCUS:
            {
                WindowFocusLostEvent windowFocusLost;
                windowRef->m_Data.IsFocused = false;
                windowRef->m_Data.WindowEventCallback(windowFocusLost);
                break;
            }

            case WM_LBUTTONDOWN:
            {
                MouseButtonDownEvent mouseButtonDown(static_cast<uint32_t>(MouseButton::Left), x, y);
                windowRef->m_Data.InputEventCallback(mouseButtonDown);
                break;
            }

            case WM_RBUTTONDOWN:
            {
                MouseButtonDownEvent mouseButtonDown(static_cast<uint32_t>(MouseButton::Right), x, y);
                windowRef->m_Data.InputEventCallback(mouseButtonDown);
                break;
            }

            case WM_MBUTTONDOWN:
            {
                MouseButtonDownEvent mouseButtonDown(static_cast<uint32_t>(MouseButton::Middle), x, y);
                windowRef->m_Data.InputEventCallback(mouseButtonDown);
                break;
            }

            case WM_XBUTTONDOWN:
            {
                uint32_t type = static_cast<uint32_t>(GET_XBUTTON_WPARAM(wParam) == XBUTTON1 ? MouseButton::X1 : MouseButton::X2);
                MouseButtonDownEvent mouseButtonDown(type, x, y);
                windowRef->m_Data.InputEventCallback(mouseButtonDown);
                break;
            }

            case WM_LBUTTONUP:
            {
                MouseButtonUpEvent mouseButtonUp(static_cast<uint32_t>(MouseButton::Left), x, y);
                windowRef->m_Data.InputEventCallback(mouseButtonUp);
                break;
            }

            case WM_RBUTTONUP:
            {
                MouseButtonUpEvent mouseButtonUp(static_cast<uint32_t>(MouseButton::Right), x, y);
                windowRef->m_Data.InputEventCallback(mouseButtonUp);
                break;
            }

            case WM_MBUTTONUP:
            {
                MouseButtonUpEvent mouseButtonUp(static_cast<uint32_t>(MouseButton::Middle), x, y);
                windowRef->m_Data.InputEventCallback(mouseButtonUp);
                break;
            }

            case WM_XBUTTONUP:
            {
                uint32_t type = static_cast<uint32_t>(GET_XBUTTON_WPARAM(wParam) == XBUTTON1 ? MouseButton::X1 : MouseButton::X2);
                MouseButtonUpEvent mouseButtonUp(type, x, y);
                windowRef->m_Data.InputEventCallback(mouseButtonUp);
                break;
            }

            case WM_MOUSEWHEEL:
            {
                int32_t delta = (int32_t)GET_WHEEL_DELTA_WPARAM(wParam);
                MouseWheelEvent mouseWheel(delta, x, y);
                windowRef->m_Data.InputEventCallback(mouseWheel);
                break;
            }

            case WM_INPUT:
            { 
                UINT dwSize = 0;
                UINT result = GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
                if (result == (UINT)-1 || dwSize == 0)
                {
                    break;
                }
                
                BYTE buffer[1024];
                result = GetRawInputData((HRAWINPUT)lParam, RID_INPUT, buffer, &dwSize, sizeof(RAWINPUTHEADER));
                if (result != dwSize)
                {
                    break;
                }

                RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(buffer);
                switch (raw->header.dwType)
                {
                    case RIM_TYPEMOUSE:
                    {
                        windowRef->m_Data.AccumulatedMouseDeltaX += static_cast<float>(raw->data.mouse.lLastX);
                        windowRef->m_Data.AccumulatedMouseDeltaY += static_cast<float>(raw->data.mouse.lLastY);                        
                        break;
                    }
                }
                break;
            }

            case WM_MOUSEMOVE:
            {
                MouseMoveEvent mouseMove(x, y);
                windowRef->m_Data.InputEventCallback(mouseMove);
                break;
            }

            case WM_KEYDOWN:
            {
                uint32_t key = static_cast<uint32_t>(wParam);
                KeyDownEvent keyDown(key);
                windowRef->m_Data.InputEventCallback(keyDown);
                break;
            }

            case WM_KEYUP:
            {
                uint32_t key = static_cast<uint32_t>(wParam);
                KeyUpEvent keyUp(key);
                windowRef->m_Data.InputEventCallback(keyUp);
                break;
            }
        }

        return DefWindowProcA(window, msg, wParam, lParam);
    }

    glm::vec2 WindowsWindow::GetMousePosition() const
    {
        POINT point;
        ScreenToClient(m_Window, &point);

        return { static_cast<float>(point.x), static_cast<float>(point.y) };
    }
}
