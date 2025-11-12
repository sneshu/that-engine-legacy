//
// File: Camera.hpp
// Description: ECS component storing camera field of view and clipping planes
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
        struct Camera
        {
            float Fov = 90.0f;
            float NearPlane = 0.01f;
            float FarPlane = 100.0f;
        };
    }
}
