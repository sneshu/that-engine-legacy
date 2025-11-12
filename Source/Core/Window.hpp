//
// File: Window.hpp
// Description: Abstract class for managing operating system window,
//              contains base window properties and event callbacks         
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Core/PCH.hpp"
#include "Core/Event/Event.hpp"

namespace ThatEngine
{
    using EventCallbackFn = std::function<void(Event&)>;

    struct WindowProperties
    {
        std::string Title;
        uint32_t InnerWidth;
        uint32_t InnerHeight;
        bool IsFocused;
    
        WindowProperties(const std::string& title = "That Engine", uint32_t width = 1600, uint32_t height = 900, bool isFocused = true)
            : Title(title), InnerWidth(width), InnerHeight(height), IsFocused(isFocused)
        {
            
        }
    };
    
    class Window
    {
        public:
        static Unique<Window> Create(const WindowProperties& properties = WindowProperties());
        
        virtual ~Window() = default;
        virtual bool IsMinimized() const = 0;
        virtual bool IsFocused() const = 0;
        virtual bool IsCursorLocked() const = 0;
        virtual const std::string& GetTitle() const = 0;
        virtual uint32_t GetInnerWidth() const = 0;
        virtual uint32_t GetInnerHeight() const = 0;
        virtual uint32_t GetOuterWidth() const = 0;
        virtual uint32_t GetOuterHeight() const = 0;
        virtual void* GetNativeWindow() const = 0;
        virtual void SetWindowEventCallback(const EventCallbackFn& callback) = 0;
        virtual void SetInputEventCallback(const EventCallbackFn& callback) = 0;
        virtual glm::vec2 GetMousePosition() const = 0;
        
        virtual void Update() = 0;
        virtual void UpdateMouseDelta() = 0;
        virtual void LockCursor() = 0;
        virtual void UnlockCursor() = 0;
        virtual void ShowError(const char* errorMessage) const = 0;

        protected:
        virtual void UpdateSizeData() = 0;
    };
}
