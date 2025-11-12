//
// File: Application.cpp
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#include "Core/PCH.hpp"
#include "Core/Application.hpp"
#include "Core/Event/EventDispatcher.hpp"
#include "Utils/MemoryUtils.hpp"

namespace ThatEngine
{
    Application::Application()
    {
        Singleton();
        m_IsRunning = true;
    }

    bool Application::Init()
    {
        Log::Get().Init();

        m_Jobs = CreateUnique<JobManager>();
        m_Jobs->Init();

        WindowProperties properties = {};
        properties.Title = "Voxadion";
        properties.InnerWidth = 1600;
        properties.InnerHeight = 900;
        properties.IsFocused = false;
        
        m_Window = Window::Create(properties);
        m_Window->SetWindowEventCallback(BIND_EVENT_FN(Application::OnEvent));
        
        Input::Get().Init(m_Window.get());
        
        m_Resources = CreateUnique<ResourceManager>();
        
        m_Renderer = CreateUnique<Renderer>();
        m_Renderer->Init(m_Window.get(), m_Resources.get(), m_StatsTracker);

        m_World = CreateUnique<World>();
        m_World->Init(m_Window.get(), m_Resources.get(), m_Jobs.get(), m_Renderer.get(), m_StatsTracker);

        return true;
    }

    void Application::Run()
    {
        THAT_CORE_INFO("Application: Initialization is complete! It took {:.2f} seconds.", m_Timer.GetElapsedTime().GetSeconds());

        while(m_IsRunning)
        {
            m_StatsTracker.StartCpuMeasurement();

            // Tracking
            UpdateMemoryUsage();
            
            // Calculations
            Timestep deltaTime = m_Timer.GetDeltaTime();
            m_Window->Update();
            m_World->Update(deltaTime);
            
            // Input handling
            HandleCursorLock();
            Input::Get().Restore();

            // Rendering
            m_World->Render();

            m_StatsTracker.StopCpuMeasurement();
        }

        m_Jobs->Shutdown();
        m_Renderer->Shutdown();
    }

    void Application::Close()
    {
        m_IsRunning = false;
    }

    void Application::HandleCursorLock()
    {
        if (Input::KeyPressed(Key::Escape))
        {
            if (m_Window->IsCursorLocked())
            {
                m_Window->UnlockCursor();
            }
            else
            {
                Close();
            }
        } 
        
        if (m_Window->IsFocused() && Input::MouseButtonPressed(MouseButton::Left))
        {
            m_Window->LockCursor();
        }
    }

    void Application::UpdateMemoryUsage()
    {
        float elapsedTime = m_Timer.GetElapsedTime().GetSeconds();
        if (elapsedTime < m_UpdateMemoryUsageTime) return;
        
        float memoryUsage = Utils::Memory::GetCurrentProcessMemoryUsage();
        m_StatsTracker.SetRamUsage(memoryUsage); 

        m_UpdateMemoryUsageTime = elapsedTime + 0.5f;
    }

    void Application::OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);

        dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
        dispatcher.Dispatch<WindowMoveEvent>(BIND_EVENT_FN(Application::OnWindowMove));
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));
        dispatcher.Dispatch<WindowFocusGainedEvent>(BIND_EVENT_FN(Application::OnWindowFocusGained));
        dispatcher.Dispatch<WindowFocusLostEvent>(BIND_EVENT_FN(Application::OnWindowFocusLost));
    }

    bool Application::OnWindowClose(WindowCloseEvent& event)
    {
        Close();
        return true;
    }

    bool Application::OnWindowMove(WindowMoveEvent& event)
    {
        return true;
    }

    bool Application::OnWindowResize(WindowResizeEvent& event)
    {         
        m_Renderer->OnWindowResize(event);
        return true;
    }

    bool Application::OnWindowFocusGained(WindowFocusGainedEvent& event)
    {
        m_Window->LockCursor();
        return true;
    }

    bool Application::OnWindowFocusLost(WindowFocusLostEvent& event)
    {
        m_Window->UnlockCursor();
        return true;
    }
}
