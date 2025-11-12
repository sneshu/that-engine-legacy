//
// File: PerformanceMonitor.hpp
// Description: ECS component for performance stats monitor system
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Types/ECSTypes.hpp"

namespace ThatEngine
{
    namespace ECS
    {
        struct PerformanceMonitor  
        {
            ECS::Entity RenderModeEntity;
            ECS::Entity CpuTimeEntity;
            ECS::Entity GpuTimeEntity;
            ECS::Entity RamUsageEntity;
            ECS::Entity FpsCounterEntity;
        };
    }
}
