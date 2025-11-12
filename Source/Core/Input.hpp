//
// File: Input.hpp
// Description: Singleton class that handles keyboard and mouse events,
//              provides static access to key / mouse button states and mouse position data
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Core/Pattern/Singleton.hpp"
#include "Core/Window.hpp"
#include "Core/Event/EventDispatcher.hpp"
#include "Core/Event/MouseEvent.hpp"
#include "Core/Event/KeyboardEvent.hpp"
#include "Core/KeyCodes.hpp"

namespace ThatEngine
{
    class Input : public Singleton<Input>
    {
        friend class Singleton<Input>;

        public:
        void Init(Window* window);
        void Restore();

        static inline bool IsKeyDown(Key key) { return Input::Get().m_KeyDown.test(static_cast<uint32_t>(key)); }
        static inline bool KeyPressed(Key key) { return Input::Get().m_KeyPressed.test(static_cast<uint32_t>(key)); }
        static inline bool KeyReleased(Key key) { return Input::Get().m_KeyReleased.test(static_cast<uint32_t>(key)); }
        static inline bool IsMouseButtonDown(MouseButton button) { return Input::Get().m_MouseButtonDown.test(static_cast<uint32_t>(button)); }
        static inline bool MouseButtonPressed(MouseButton button) { return Input::Get().m_MouseButtonPressed.test(static_cast<uint32_t>(button)); }
        static inline glm::vec2 GetMousePosition() { return Input::Get().m_MousePosition; }
        static inline float GetMousePositionX() { return Input::Get().m_MousePosition.x; }
        static inline float GetMousePositionY() { return Input::Get().m_MousePosition.y; }
        static inline glm::vec2 GetMousePositionDelta() { return Input::Get().m_MousePositionDelta; }
        static inline float GetMousePositionDeltaX() { return Input::Get().m_MousePositionDelta.x; }
        static inline float GetMousePositionDeltaY() { return Input::Get().m_MousePositionDelta.y; }

        private:
        void OnEvent(Event& event);
        bool OnMouseButtonDown(MouseButtonDownEvent& event);
        bool OnMouseButtonUp(MouseButtonUpEvent& event);
        bool OnMouseWheel(MouseWheelEvent& event);
        bool OnMouseMove(MouseMoveEvent& event);
        bool OnRawMouseMove(RawMouseMoveEvent& event);
        bool OnKeyDown(KeyDownEvent& event);
        bool OnKeyUp(KeyUpEvent& event);

        protected:
        Input() = default;
        std::bitset<256> m_KeyDown;
        std::bitset<256> m_KeyPressed;
        std::bitset<256> m_KeyReleased;
        std::bitset<8> m_MouseButtonDown;
        std::bitset<8> m_MouseButtonPressed;
        std::bitset<8> m_MouseButtonReleased;
        glm::vec2 m_MousePositionDelta;
        glm::vec2 m_MousePosition; 
    };
}