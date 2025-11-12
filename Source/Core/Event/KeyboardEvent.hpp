//
// File: KeyboardEvent.hpp
// Description: Holds all keyboard-related operating system events
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Core/Event/Event.hpp"
#include "Core/KeyCodes.hpp"

namespace ThatEngine
{
    class KeyDownEvent : public Event
    {
        public:
        KeyDownEvent(uint32_t key) 
            : m_Key(key) {}

        inline uint32_t GetKey() const { return m_Key; }
        inline std::string ToString() const override 
        {
            std::stringstream ss;
            ss << GetName() << " { Key = " << m_Key << " }";
            return ss.str();
        }
        DEFINE_EVENT_TYPE(KeyDown);

        private:
        uint32_t m_Key;
    };

    class KeyUpEvent : public Event
    {
        public:
        KeyUpEvent(uint32_t key) 
            : m_Key(key) {}

        inline uint32_t GetKey() const { return m_Key; }
        inline std::string ToString() const override 
        {
            std::stringstream ss;
            ss << GetName() << " { Key = " << m_Key << " }";
            return ss.str();
        }
        DEFINE_EVENT_TYPE(KeyUp);

        private:
        uint32_t m_Key;
    };
    
}
