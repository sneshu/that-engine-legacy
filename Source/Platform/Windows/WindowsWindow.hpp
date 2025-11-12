//
// File: WindowsWindow.hpp
// Description: 
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Core/PCH.hpp"
#include "Core/Window.hpp"
#include "Core/Event/EventDispatcher.hpp"

namespace ThatEngine
{    
    struct WindowData
    {
        std::string Title;
        uint32_t PositionX;
        uint32_t PositionY;
        uint32_t InnerWidth;
        uint32_t InnerHeight;
        uint32_t OuterWidth;
        uint32_t OuterHeight;
        RECT ClipArea;
        bool IsMinimized;
        bool IsFocused;
        bool IsCursorLocked;
        EventCallbackFn WindowEventCallback;
        EventCallbackFn InputEventCallback;
        float AccumulatedMouseDeltaX;
        float AccumulatedMouseDeltaY;
    };

    class WindowsWindow : public Window
    {
        public:
        WindowsWindow(const WindowProperties& properties);
        ~WindowsWindow();

        inline const std::string& GetTitle() const override { return m_Data.Title; }
        inline bool IsMinimized() const override { return m_Data.IsMinimized; }
        inline bool IsFocused() const override { return m_Data.IsFocused; }
        inline bool IsCursorLocked() const override { return m_Data.IsCursorLocked; }
        inline uint32_t GetInnerWidth() const override { return m_Data.InnerWidth; }
        inline uint32_t GetInnerHeight() const override { return m_Data.InnerHeight; }
        inline uint32_t GetOuterWidth() const override { return m_Data.OuterWidth; }
        inline uint32_t GetOuterHeight() const override { return m_Data.OuterHeight; }
        inline void* GetNativeWindow() const override { return m_Window; }
        inline void SetWindowEventCallback(const EventCallbackFn& callback) override { m_Data.WindowEventCallback = callback; }
        inline void SetInputEventCallback(const EventCallbackFn& callback) override { m_Data.InputEventCallback = callback; }
        glm::vec2 GetMousePosition() const override;
        
        void Update() override;
        void UpdateMouseDelta() override;
        void LockCursor() override;
        void UnlockCursor() override;
        void ShowError(const char* message) const override;
        static LRESULT CALLBACK EventCallback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

        protected:
        void UpdateSizeData() override;

        private:
        virtual bool Init(const WindowProperties& properties);

        private:
        WindowData m_Data;
        HWND m_Window;
    };
}
