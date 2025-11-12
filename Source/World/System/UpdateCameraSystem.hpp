//
// File: UpdateCameraSystem.hpp
// Description: ECS system that invokes world's active camera's view projection matrix recalculation  
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Core/Window.hpp"
#include "World/Component/Transform.hpp"
#include "World/Component/Camera.hpp"

namespace ThatEngine
{
    namespace ECS
    {
        void UpdateCameraSystem(ECS::Registry& registry, Timestep deltaTime)
        {
            World* world = registry.ctx().get<World*>();
            Entity entity = world->GetActiveCamera();

            const auto& transform = registry.get<ECS::Transform>(entity);
            const auto& camera = registry.get<ECS::Camera>(entity);

            world->UpdateViewProjection(transform, camera);
        }
    }
}