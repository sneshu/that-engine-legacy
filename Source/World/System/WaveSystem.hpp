//
// File: WaveSystem.hpp
// Description: ECS system that is used for object waves
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Core/PCH.hpp"
#include "Types/ECSTypes.hpp"
#include "World/World.hpp"
#include "World/Component/Transform.hpp"
#include "World/Component/Wave.hpp"

namespace ThatEngine
{
    namespace ECS
    {
        void WaveSystem(ECS::Registry& registry, Timestep deltaTime)
        {
            auto view = registry.view<ECS::Transform, ECS::Wave>();
            World* world = registry.ctx().get<World*>();
            float elapsedTime = world->GetGlobalData().Time;

            view.each([&](auto& transform, const auto& wave)
            {
                float sinValue = glm::sin(wave.Frequency * elapsedTime + wave.FrequencyOffset);
                float cosValue = glm::cos(wave.Frequency * elapsedTime + wave.FrequencyOffset);

                float positionY = 0.5f * (sinValue * wave.MaxHeight + cosValue * wave.MaxHeight);
                transform.SetPosition(transform.Position.x, positionY, transform.Position.z);
            });
        }
    }
}