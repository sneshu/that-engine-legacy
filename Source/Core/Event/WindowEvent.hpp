//
// File: WindowEvent.hpp
// Description: : Holds all window-related operating system events
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Core/Event/Event.hpp"
#include "Core/Defines.hpp"

namespace ThatEngine
{
    class WindowCloseEvent : public Event
    {
        public:
        WindowCloseEvent() = default;
        DEFINE_EVENT_TYPE(WindowClose);
    };
    
    class WindowMoveEvent: public Event
    {
        public:
        WindowMoveEvent(uint32_t positionX, uint32_t positionY) 
            : m_PositionX(positionX), m_PositionY(positionY) {}
            
        inline uint32_t GetX() const { return m_PositionX; }
        inline uint32_t GetY() const { return m_PositionY; }
        inline std::string ToString() const override 
        {
            std::stringstream ss;
            ss << GetName() << " { X = " << m_PositionX << ", Y = " << m_PositionY << " }";
            return ss.str();
        }
        DEFINE_EVENT_TYPE(WindowMove);

        private:
        uint32_t m_PositionX;
        uint32_t m_PositionY;
    };

    class WindowResizeEvent : public Event
    {
        public:
        WindowResizeEvent(uint32_t width, uint32_t height) 
            : m_Width(width), m_Height(height) {}
            
        inline uint32_t GetWidth() const { return m_Width; }
        inline uint32_t GetHeight() const { return m_Height; }
        inline std::string ToString() const override 
        {
            std::stringstream ss;
            ss << GetName() << " { Width = " << m_Width << ", Height = " << m_Height << " }";
            return ss.str();
        }
        DEFINE_EVENT_TYPE(WindowResize);

        private:
        uint32_t m_Width;
        uint32_t m_Height;
    };

    class WindowFocusGainedEvent : public Event
    {
        public:
        WindowFocusGainedEvent() = default;
        DEFINE_EVENT_TYPE(WindowFocusGained);
    };

    class WindowFocusLostEvent : public Event
    {
        public:
        WindowFocusLostEvent() = default;
        DEFINE_EVENT_TYPE(WindowFocusLost);
    };
}
