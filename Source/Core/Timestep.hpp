//
// File: Timestep.hpp
// Description: Utility class that encapsulates a time interval and provides time conversions
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

namespace ThatEngine
{
    class Timestep
    {
        public:
        Timestep(float time = 0.0f) : m_Time(time) {}

        inline float GetSeconds() const { return m_Time; }
        inline float GetMilliseconds() const { return m_Time * 1000.0f; }

        private:
        float m_Time;
    };
}
