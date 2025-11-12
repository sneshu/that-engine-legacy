//
// File: MathUtils.hpp
// Description: Math related utils
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include <glm/glm.hpp>

namespace ThatEngine
{
    namespace Utils
    {
        namespace Math
        {
            float CubicLerp(float a, float b, float weight)
            {
                return a + (b - a) * (3.0f - weight * 2.0f) * weight * weight;
            }

            float Lerp(float a, float b, float weight)
            {
                return a + (b - a) * weight;
            }

            template <typename T>
            inline T Remap(T aMin, T aMax, T bMin, T bMax, T weight) 
            {
                return bMin + (weight - aMin) * (bMax - bMin) / (aMax - aMin);
            }
        }  
    }
} 
