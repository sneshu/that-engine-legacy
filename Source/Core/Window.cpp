//
// File: Window.cpp
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#include "Core/PCH.hpp"
#include "Core/Window.hpp"
#ifdef PLATFORM_WINDOWS
    #include "Platform/Windows/WindowsWindow.hpp"
#endif

namespace ThatEngine
{
    Unique<Window> Window::Create(const WindowProperties& properties)
    {
        #ifdef PLATFORM_WINDOWS
        return CreateUnique<WindowsWindow>(properties);
        #endif
    }
}