//
// File: GeometryUtils.hpp
// Description: Geometry related utils
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
        namespace Geometry
        {
            struct Plane 
            {
                glm::vec3 Normal;
                float Distance;

                void Normalize() 
                {
                    float length = glm::length(Normal);
                    Normal /= length;
                    Distance /= length;
                }
            };

            inline void ExtractFrustumPlanes(const glm::mat4& viewProjection, Plane planes[6])
            {
                // Left plane
                planes[0].Normal.x = viewProjection[0][3] + viewProjection[0][0];
                planes[0].Normal.y = viewProjection[1][3] + viewProjection[1][0];
                planes[0].Normal.z = viewProjection[2][3] + viewProjection[2][0];
                planes[0].Distance = viewProjection[3][3] + viewProjection[3][0];

                // Right plane
                planes[1].Normal.x = viewProjection[0][3] - viewProjection[0][0];
                planes[1].Normal.y = viewProjection[1][3] - viewProjection[1][0];
                planes[1].Normal.z = viewProjection[2][3] - viewProjection[2][0];
                planes[1].Distance = viewProjection[3][3] - viewProjection[3][0];

                // Bottom plane
                planes[2].Normal.x = viewProjection[0][3] + viewProjection[0][1];
                planes[2].Normal.y = viewProjection[1][3] + viewProjection[1][1];
                planes[2].Normal.z = viewProjection[2][3] + viewProjection[2][1];
                planes[2].Distance = viewProjection[3][3] + viewProjection[3][1];

                // Top plane
                planes[3].Normal.x = viewProjection[0][3] - viewProjection[0][1];
                planes[3].Normal.y = viewProjection[1][3] - viewProjection[1][1];
                planes[3].Normal.z = viewProjection[2][3] - viewProjection[2][1];
                planes[3].Distance = viewProjection[3][3] - viewProjection[3][1];

                // Near plane
                planes[4].Normal.x = viewProjection[0][3] + viewProjection[0][2];
                planes[4].Normal.y = viewProjection[1][3] + viewProjection[1][2];
                planes[4].Normal.z = viewProjection[2][3] + viewProjection[2][2];
                planes[4].Distance = viewProjection[3][3] + viewProjection[3][2];

                // Far plane
                planes[5].Normal.x = viewProjection[0][3] - viewProjection[0][2];
                planes[5].Normal.y = viewProjection[1][3] - viewProjection[1][2];
                planes[5].Normal.z = viewProjection[2][3] - viewProjection[2][2];
                planes[5].Distance = viewProjection[3][3] - viewProjection[3][2];

                // Normalize planes
                for (int i = 0; i < 6; i++)
                {
                    planes[i].Normalize();
                }
            }

            inline bool IsPointInsideRect(const glm::vec3& position, float x, float y, float width, float height)
            {
                return (position.x >= x && position.x <= width) && (position.y >= y && position.y <= height);
            }

            inline bool IsSphereInsideFrustum(const glm::vec3& position, float radius, Plane planes[6])
            {
                for (int i = 0; i < 6; i++)
                {
                    float distance = glm::dot(planes[i].Normal, position) + planes[i].Distance;
                    if (distance < -radius) return false;
                }

                return true; 
            }
        }
    }
}