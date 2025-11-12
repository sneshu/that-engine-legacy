//
// File: Input.cpp
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#include "Core/PCH.hpp"
#include "Core/Input.hpp"
#include "Core/Event/EventDispatcher.hpp"
#include "Core/Event/MouseEvent.hpp"
#include "Core/Event/KeyboardEvent.hpp"

namespace ThatEngine
{
    void Input::Init(Window* window)
    {
        window->SetInputEventCallback(BIND_EVENT_FN(Input::OnEvent));
        m_MousePosition = window->GetMousePosition();
    }

    void Input::Restore()
    {
        m_KeyPressed.reset();
        m_KeyReleased.reset();
        m_MouseButtonPressed.reset();
        m_MouseButtonReleased.reset();
        m_MousePositionDelta.x = 0;
        m_MousePositionDelta.y = 0;
    }

    void Input::OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);

        dispatcher.Dispatch<MouseButtonDownEvent>(BIND_EVENT_FN(Input::OnMouseButtonDown));
        dispatcher.Dispatch<MouseButtonUpEvent>(BIND_EVENT_FN(Input::OnMouseButtonUp));
        dispatcher.Dispatch<MouseWheelEvent>(BIND_EVENT_FN(Input::OnMouseWheel));
        dispatcher.Dispatch<MouseMoveEvent>(BIND_EVENT_FN(Input::OnMouseMove));
        dispatcher.Dispatch<RawMouseMoveEvent>(BIND_EVENT_FN(Input::OnRawMouseMove));
        dispatcher.Dispatch<KeyDownEvent>(BIND_EVENT_FN(Input::OnKeyDown));
        dispatcher.Dispatch<KeyUpEvent>(BIND_EVENT_FN(Input::OnKeyUp));
    }

    bool Input::OnMouseButtonDown(MouseButtonDownEvent& event)
    {
        uint32_t code = event.GetButtonType();

        // Prevent loops
        if (!m_MouseButtonDown.test(code))
        {
            m_MouseButtonPressed.set(code, true);
        }

        m_MouseButtonDown.set(code, true);

        return true;
    }

    bool Input::OnMouseButtonUp(MouseButtonUpEvent& event)
    {
        uint32_t code = event.GetButtonType();
        m_MouseButtonDown.set(code, false);
        m_MouseButtonReleased.set(code, true);

        return true;
    }

    bool Input::OnMouseWheel(MouseWheelEvent& event)
    {
        return true;
    }

    bool Input::OnMouseMove(MouseMoveEvent& event)
    {
        float x = static_cast<float>(event.GetX());
        float y = static_cast<float>(event.GetY());
        
        m_MousePosition.x = x;
        m_MousePosition.y = y;

        return true;
    }

    bool Input::OnRawMouseMove(RawMouseMoveEvent& event)
    {
        m_MousePositionDelta.x = event.GetX();
        m_MousePositionDelta.y = event.GetY();

        return true;
    }

    bool Input::OnKeyDown(KeyDownEvent& event)
    {
        uint32_t code = event.GetKey();
        
        // Prevent loops
        if (!m_KeyDown.test(code))
        {
            m_KeyPressed.set(code, true);
        }

        m_KeyDown.set(code, true);

        return true;
    }

    bool Input::OnKeyUp(KeyUpEvent& event)
    {
        uint32_t code = event.GetKey();
        m_KeyDown.set(code, false);
        m_KeyReleased.set(code, true);

        return true;
    }
}