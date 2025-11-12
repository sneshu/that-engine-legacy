//
// File: Timer.hpp
// Description: Utility class for measuring elapsed time and delta time
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Core/Timestep.hpp"
#include <chrono>

namespace ThatEngine
{
    class Timer
    {
        public:
        Timer() 
        {
            Reset();
        }

        void Reset()
        {
            m_StartTime = std::chrono::high_resolution_clock::now();
            m_LastTime = m_StartTime;
        }

        Timestep GetElapsedTime() const
        {
            auto currentTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float> elapsedTime = currentTime - m_StartTime;

            return Timestep(elapsedTime.count());
        }

        Timestep GetDeltaTime()
        {
            auto currentTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float> deltaTime = currentTime - m_LastTime;
            m_LastTime = currentTime;

            return Timestep(deltaTime.count());
        }

        private:
        std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_LastTime;
    };
}