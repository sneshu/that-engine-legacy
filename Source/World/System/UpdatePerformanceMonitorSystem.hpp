//
// File: UpdatePerformanceMonitorSystem.hpp
// Description: ECS system that updates the performance monitor text components
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Core/PCH.hpp"
#include "Core/Input.hpp"
#include "Core/StatsTracker.hpp"
#include "Renderer/Renderer.hpp"
#include "Types/ECSTypes.hpp"
#include "World/Component/PerformanceMonitor.hpp"
#include "World/Component/Lifetime.hpp"
#include "World/Component/Text.hpp"

namespace ThatEngine
{
    namespace ECS
    {
        void UpdatePerformanceMonitorSystem(ECS::Registry& registry, Timestep deltaTime)
        {
            auto view = registry.view<ECS::PerformanceMonitor, ECS::Lifetime>();
            const auto stats = registry.ctx().get<StatsTracker*>();
            Renderer* renderer = registry.ctx().get<Renderer*>();

            view.each([&](const auto& performanceMonitor, auto& lifetime) 
            {
                bool forceUpdate = false;

                // User input
                {
                    if (Input::KeyPressed(Key::GraveAccent))
                    {
                        registry.get<ECS::Transform>(performanceMonitor.RenderModeEntity).Toggle();
                        registry.get<ECS::Transform>(performanceMonitor.CpuTimeEntity).Toggle();
                        registry.get<ECS::Transform>(performanceMonitor.GpuTimeEntity).Toggle();
                        registry.get<ECS::Transform>(performanceMonitor.RamUsageEntity).Toggle();
                        registry.get<ECS::Transform>(performanceMonitor.FpsCounterEntity).Toggle();
                    }

                    else if (Input::KeyPressed(Key::D1))
                    {
                        renderer->SetRenderMode(RenderMode::Color);
                        forceUpdate = true;
                    }

                    else if (Input::KeyPressed(Key::D2))
                    {
                        renderer->SetRenderMode(RenderMode::Depth);
                        forceUpdate = true;
                    }

                    else if (Input::KeyPressed(Key::D3))
                    {
                        renderer->SetRenderMode(RenderMode::Normals);
                        forceUpdate = true;
                    }

                    else if (Input::KeyPressed(Key::D4))
                    {
                        renderer->SetRenderMode(RenderMode::Triangles);
                        forceUpdate = true;
                    }

                    else if (Input::KeyPressed(Key::D5))
                    {
                        renderer->SetRenderMode(RenderMode::Wireframe);
                        forceUpdate = true;
                    }
                }      

                // Update values
                {
                    if (!forceUpdate && lifetime.Timer.GetElapsedTime().GetSeconds() < 0.5f) return;

                    auto& renderModeText = registry.get<ECS::Text>(performanceMonitor.RenderModeEntity);
                    auto& cpuTimeText = registry.get<ECS::Text>(performanceMonitor.CpuTimeEntity);
                    auto& gpuTimeText = registry.get<ECS::Text>(performanceMonitor.GpuTimeEntity);
                    auto& ramUsageText = registry.get<ECS::Text>(performanceMonitor.RamUsageEntity);
                    auto& fpsCounterText = registry.get<ECS::Text>(performanceMonitor.FpsCounterEntity);
                    
                    const char* renderMode = ToString(renderer->GetRenderMode());
                    renderModeText.Content = std::format("{}_MODE", renderMode);

                    float cpuTime = stats->GetCpuTime();
                    cpuTimeText.Content = std::format("CPU: {:.3f} ms", cpuTime);
                    
                    float gpuTime = stats->GetGpuTime();
                    gpuTimeText.Content = std::format("GPU: {:.3f} ms", gpuTime);

                    float ramUsage = stats->GetRamUsage();
                    ramUsageText.Content = std::format("RAM: {:.1f} MB", ramUsage);
                    
                    float fps = 1.0f / deltaTime.GetSeconds();
                    fpsCounterText.Content = std::format("FPS: {:.1f}", fps);

                    lifetime.Timer.Reset();
                }
            });
        }
    }
}