//
// File: MouseEvent.hpp
// Description: Holds all mouse-related operating system events
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Core/Event/Event.hpp"
#include "Core/Defines.hpp"

namespace ThatEngine
{
    enum class MouseButton : uint32_t
    {
        Left = 0,
        Right = 1,
        Middle = 2,
        X1 = 3,
        X2= 4
    };

    class MouseButtonDownEvent : public Event
    {
        public:
        MouseButtonDownEvent(uint32_t buttonType, uint32_t xPosition, uint32_t yPosition) 
            : m_ButtonType(buttonType), m_XPosition(xPosition), m_YPosition(yPosition) {}

        inline uint32_t GetButtonType() const { return m_ButtonType; }
        inline uint32_t GetX() const { return m_XPosition; }
        inline uint32_t GetY() const { return m_YPosition; }
        inline std::string ToString() const override 
        {
            std::stringstream ss;
            ss << GetName() << " { ButtonType = " << m_ButtonType << ", X = " << m_XPosition << ", Y = " << m_YPosition << " }";
            return ss.str();
        }
        DEFINE_EVENT_TYPE(MouseButtonDown);

        private:
        uint32_t m_ButtonType;
        uint32_t m_XPosition;
        uint32_t m_YPosition;
    };

    class MouseButtonUpEvent : public Event
    {
        public:
        MouseButtonUpEvent(uint32_t buttonType, uint32_t xPosition, uint32_t yPosition) 
            : m_ButtonType(buttonType), m_XPosition(xPosition), m_YPosition(yPosition) {}

        inline uint32_t GetButtonType() const { return m_ButtonType; }
        inline uint32_t GetX() const { return m_XPosition; }
        inline uint32_t GetY() const { return m_YPosition; }
        inline std::string ToString() const override 
        {
            std::stringstream ss;
            ss << GetName() << " { ButtonType = " << m_ButtonType << ", X = " << m_XPosition << ", Y = " << m_YPosition << " }";
            return ss.str();
        }
        DEFINE_EVENT_TYPE(MouseButtonUp);

        private:
        uint32_t m_ButtonType;
        uint32_t m_XPosition;
        uint32_t m_YPosition;
    };
    
    class MouseWheelEvent : public Event
    {
        public:
        MouseWheelEvent(int32_t delta, uint32_t xPosition, uint32_t yPosition) 
            : m_Delta(delta), m_XPosition(xPosition), m_YPosition(yPosition) {}

        inline int32_t GetDelta() const { return m_Delta; }
        inline uint32_t GetX() const { return m_XPosition; }
        inline uint32_t GetY() const { return m_YPosition; }
        inline std::string ToString() const override 
        {
            std::stringstream ss;
            ss << GetName() << " { Delta = " << m_Delta << ", X = " << m_XPosition << ", Y = " << m_YPosition << " }";
            return ss.str();
        }
        DEFINE_EVENT_TYPE(MouseWheel);

        private:
        int32_t m_Delta;
        uint32_t m_XPosition;
        uint32_t m_YPosition;
    };

    class MouseMoveEvent : public Event
    {
        public:
        MouseMoveEvent(uint32_t xPosition, uint32_t yPosition) 
            : m_XPosition(xPosition), m_YPosition(yPosition) {}

        inline uint32_t GetX() const { return m_XPosition; }
        inline uint32_t GetY() const { return m_YPosition; }
        inline std::string ToString() const override 
        {
            std::stringstream ss;
            ss << GetName() << " { X = " << m_XPosition << ", Y = " << m_YPosition << " }";
            return ss.str();
        }
        DEFINE_EVENT_TYPE(MouseMove);

        private:
        uint32_t m_XPosition;
        uint32_t m_YPosition;
    };

    class RawMouseMoveEvent : public Event
    {
        public:
        RawMouseMoveEvent(float xDelta, float yDelta) 
            : m_XDelta(xDelta), m_YDelta(yDelta) {}

        inline float GetX() const { return m_XDelta; }
        inline float GetY() const { return m_YDelta; }
        inline std::string ToString() const override 
        {
            std::stringstream ss;
            ss << GetName() << " { X = " << m_XDelta << ", Y = " << m_YDelta << " }";
            return ss.str();
        }
        DEFINE_EVENT_TYPE(RawMouseMove);

        private:
        float m_XDelta;
        float m_YDelta;
    };
}
