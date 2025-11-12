//
// File: RotateTextSystem.hpp
// Description: ECS system that is used for rotating world-space texts
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Core/PCH.hpp"
#include "Types/ECSTypes.hpp"
#include "World/World.hpp"
#include "World/Component/Transform.hpp"
#include "World/Component/WorldSpace.hpp"
#include "World/Component/Text.hpp"

namespace ThatEngine
{
    namespace ECS
    {
        void RotateTextSystem(ECS::Registry& registry, Timestep deltaTime)
        {
            auto view = registry.view<ECS::Transform, ECS::WorldSpace, ECS::Text>();

            view.each([&](auto& transform, const auto& text)
            {
                transform.Rotate(0.0f, 45.0f * deltaTime.GetSeconds(), 0.0f);
            });
        }
    }
}