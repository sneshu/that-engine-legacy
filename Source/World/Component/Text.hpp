//
// File: Text.hpp
// Description: ECS component used for text display
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Types/FontTypes.hpp"

#include <glm/glm.hpp>

namespace ThatEngine
{
    namespace ECS
    {
        struct Text
        {   
            FontAssetType Font;
            float FontSize;
            glm::vec4 Color;
            std::string Content;
        };
    }
}
