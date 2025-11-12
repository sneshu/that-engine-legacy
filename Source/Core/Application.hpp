//
// File: Application.hpp
// Description: Implements core application class that manages the main game loop,
//              owns and manages the Window, Renderer and World objects,
//              handles window events
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Core/Pattern/Singleton.hpp"
#include "Core/Window.hpp"
#include "Core/Input.hpp"
#include "Core/JobManager.hpp"
#include "Core/StatsTracker.hpp"
#include "Core/Event/WindowEvent.hpp"
#include "Core/Event/MouseEvent.hpp"
#include "Core/Event/KeyboardEvent.hpp"
#include "Core/Timestep.hpp"
#include "Core/Timer.hpp"
#include "World/World.hpp"
#include "Renderer/ResourceManager.hpp"
#include "Renderer/Renderer.hpp"

namespace ThatEngine
{
    class Application : public Singleton<Application>
    {
        friend class Singleton<Application>;

        public:
        Application();
        bool Init();
        void Run();
        void Close();

        private:
        void HandleCursorLock();
        void UpdateMemoryUsage();

        void OnEvent(Event& event);
        bool OnWindowClose(WindowCloseEvent& event);
        bool OnWindowMove(WindowMoveEvent& event);
        bool OnWindowResize(WindowResizeEvent& event);
        bool OnWindowFocusGained(WindowFocusGainedEvent& event);
        bool OnWindowFocusLost(WindowFocusLostEvent& event);

        private:
        Unique<Window> m_Window;
        Unique<JobManager> m_Jobs;
        Unique<ResourceManager> m_Resources;
        Unique<Renderer> m_Renderer;
        Unique<World> m_World;

        StatsTracker m_StatsTracker;
        Timer m_Timer;
        bool m_IsRunning;
        
        float m_UpdateMemoryUsageTime;
    };
}