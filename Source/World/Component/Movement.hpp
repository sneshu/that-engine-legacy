//
// File: Movement.hpp
// Description: ECS component storing movement related variables
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include <glm/glm.hpp>

namespace ThatEngine
{
    namespace ECS
    {
        struct Movement
        {
            bool IsMoving = false;
            bool IsSprinting = false;

            float DefaultMaxSpeed = 10.0f;
            float SprintMaxSpeed = 50.0f;
            float CurrentMaxSpeed = DefaultMaxSpeed;
        };
    }
}
