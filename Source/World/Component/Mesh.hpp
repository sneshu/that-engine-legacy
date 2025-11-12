//
// File: Mesh.hpp
// Description: ECS component storing mesh type
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Renderer/GraphicsContext.hpp"

#include <glm/glm.hpp>

namespace ThatEngine
{
    namespace ECS
    {
        struct Mesh
        {
            MeshAssetType Type = MeshAssetType::None;
        };
    }
}
