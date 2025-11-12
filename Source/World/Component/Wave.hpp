//
// File: Wave.hpp
// Description: ECS component storing wave variables
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
        struct Wave
        {
            float Frequency = 0.5f;
            float FrequencyOffset = 0.0f;
            float MaxHeight = 5.0f;
        };
    }
}
