//
// File: Event.hpp
// Description: Base class for operating system events
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#define DEFINE_EVENT_TYPE(type)                                             \
    static EventType GetStaticType() { return EventType::type; }            \
    inline EventType GetType() const override { return EventType::type; }   \
    inline const char* GetName() const override { return #type; }

namespace ThatEngine
{
    enum class EventType
    {
        None                = 0,
        WindowClose         = 0x0010,
        WindowMove          = 0x0011,
        WindowResize        = 0x0012,
        WindowFocusGained   = 0x0013,
        WindowFocusLost     = 0x0014,
        MouseMove           = 0x0100,
        RawMouseMove        = 0x0101,
        MouseButtonDown     = 0x0102,
        MouseButtonUp       = 0x0103,
        MouseWheel          = 0x0104,
        KeyDown             = 0x1000,
        KeyUp               = 0x1001
    };

    class Event
    {
        public:
        virtual EventType GetType() const = 0;
        virtual const char* GetName() const = 0;
        virtual std::string ToString() const { return GetName(); }
        virtual ~Event() = default;
        
        bool Handled = false;
    };
}