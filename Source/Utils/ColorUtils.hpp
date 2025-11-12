//
// File: ColorUtils.hpp
// Description: Color related utils
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include <glm/glm.hpp>
#include <cmath>

namespace ThatEngine
{
    namespace Utils
    {
        namespace Color
        {
            constexpr glm::vec3 ToLinear(glm::vec3 color)
            {
                return glm::vec3(std::pow(color.r, 2.2f), std::pow(color.g, 2.2f), std::pow(color.b, 2.2f));
            }

            constexpr glm::vec4 ToLinear(glm::vec4 color)
            {
                return glm::vec4(std::pow(color.r, 2.2f), std::pow(color.g, 2.2f), std::pow(color.b, 2.2f), color.a);
            }

            static const glm::vec4 Cyan = glm::vec4(ToLinear(glm::vec3(32.0f, 187.0f, 255.0f) / 255.0f), 1.0f);
            static const glm::vec4 Green = glm::vec4(ToLinear(glm::vec3(32.0f, 255.0f, 32.0f) / 255.0f), 1.0f);
            static const glm::vec4 Yellow = glm::vec4(ToLinear(glm::vec3(255.0f, 187.0f, 32.0f) / 255.0f), 1.0f);
            static const glm::vec4 Red = glm::vec4(ToLinear(glm::vec3(255.0f, 32.0f, 32.0f) / 255.0f), 1.0f);
            static const glm::vec4 Black = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        }
    }   
}