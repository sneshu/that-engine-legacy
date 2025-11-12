//
// File: EventDispatcher.hpp
// Description: Utility class responsible for dispatching events to appropriate event handlers based on event type
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Core/Defines.hpp"
#include "Core/Event/Event.hpp"

namespace ThatEngine
{
    class EventDispatcher
	{
        public:
            EventDispatcher(Event& event) : m_Event(event) {}
            
            template<typename T, typename F>
            bool Dispatch(const F& callback)
            {
                if (m_Event.GetType() == T::GetStaticType())
                {
                    m_Event.Handled |= callback(static_cast<T&>(m_Event));
                    return true;
                }
        
                return false;
            }
            
        private:
            Event& m_Event;
	};
}