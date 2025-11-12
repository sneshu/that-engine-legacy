//
// File: ECSTypes.hpp
// Description: 
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Core/Timestep.hpp"

#include <entt/entt.hpp>

namespace ThatEngine
{
    namespace ECS
    {
        using Entity = entt::entity;
        using Registry = entt::registry;
        using SystemFunction = std::function<void(ECS::Registry&, Timestep)>;
        const uint32_t MAX_ENTITIES = 65336;
    }
}