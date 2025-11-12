//
// File: StatsTracker.hpp
// Description: Measures and tracks performance stats
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Core/Timer.hpp"

namespace ThatEngine
{
    class StatsTracker
    {
        public:
        StatsTracker() = default;
        
        inline constexpr float GetCpuTime() const { return m_CpuTime; }
        inline constexpr float GetGpuTime() const { return m_GpuTime; }
        inline constexpr float GetRamUsage() const { return m_RamUsage; }

        inline void StartCpuMeasurement() { m_CpuTimer.Reset(); }
        inline void StopCpuMeasurement() { m_CpuTime = m_CpuTimer.GetElapsedTime().GetMilliseconds(); }
        inline void SetGpuTime (float milliseconds) { m_GpuTime = milliseconds; }
        inline void SetRamUsage(float usage) { m_RamUsage = usage; }  

        private:
        Timer m_CpuTimer;

        float m_CpuTime = 0.0f;
        float m_GpuTime = 0.0f;
        float m_RamUsage = 0.0f;
    };
}