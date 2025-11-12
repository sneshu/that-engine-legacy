//
// File: SystemManager.hpp
// Description: 
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Core/Timestep.hpp"
#include "Types/ECSTypes.hpp"

#include <functional>

namespace ThatEngine
{
    namespace ECS
    {
        class SystemManager
        {
            public:
            SystemManager() = default;

            void AddSystem(SystemFunction system)
            {
                m_RegisteredSystems.push_back(system);
            }

            void Update(ECS::Registry& registry, Timestep deltaTime)
            {
                for (auto& system : m_RegisteredSystems)
                {
                    system(registry, deltaTime);
                }
            }

            private:
            std::vector<SystemFunction> m_RegisteredSystems;
        };
    }
}