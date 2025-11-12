//
// File: UpdateScreenSpaceTransformSystem.hpp
// Description: ECS system that recalculates transform vectors and model matrix in screen space             
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Core/PCH.hpp"
#include "Types/ECSTypes.hpp"
#include "World/Component/Transform.hpp"
#include "Utils/GeometryUtils.hpp"

namespace ThatEngine
{
    namespace ECS
    {
        void UpdateScreenSpaceTransformSystem(ECS::Registry& registry, Timestep deltaTime)
        {
            auto view = registry.view<ECS::Transform, ECS::ScreenSpace>();
            Window* window = registry.ctx().get<Window*>();
            const float screenWidth = static_cast<float>(window->GetInnerWidth());
            const float screenHeight = static_cast<float>(window->GetInnerHeight());
        
            view.each([&](auto& transform) 
            {
                // Frustum culling
                transform.IsVisible = transform.IsActive && Utils::Geometry::IsPointInsideRect(transform.Position, 0.0f, 0.0f, screenWidth, screenHeight);

                if (!transform.IsDirty || !transform.IsVisible) return;

                // Recalculate model matrix
                glm::mat4 translation = glm::translate(glm::mat4(1.0f), transform.Position);
                glm::mat4 scale = glm::scale(glm::mat4(1.0f), transform.Scale);
                
                transform.Model = translation * scale;
            
                // Mark as clean
                transform.IsDirty = false;
            });
        }
    }
}