//
// File: Transform.hpp
// Description: ECS component storing transform and model data
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
        struct Transform
        {
            glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
            glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
            glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

            glm::vec3 Forward = { 0.0f, 0.0f, 1.0f };
            glm::vec3 Up = { 0.0f, 1.0f, 0.0f };
            glm::vec3 Right = { 1.0f, 0.0f, 0.0f };

            glm::mat4 Model;

            float BoundingRadius = glm::length(Scale) * 0.5f;
            bool IsVisible = false;
            bool IsActive = true;
            bool IsDirty = true;

            // Setters
            void SetPosition(float x, float y, float z)
            {
                Position.x = x;
                Position.y = y;
                Position.z = z;
                IsDirty = true;
            }

            void Translate(float x, float y, float z)
            {
                Position.x += x;
                Position.y += y;
                Position.z += z;
                IsDirty = true;
            }

            void Translate(const glm::vec3& translate)
            {
                Position.x += translate.x;
                Position.y += translate.y;
                Position.z += translate.z;
                IsDirty = true;
            }

            void SetRotation(float pitch, float yaw, float roll)
            {
                Rotation.x = pitch;
                Rotation.y = yaw;
                Rotation.z = roll;
                IsDirty = true;
            }

            void Rotate(float pitch, float yaw, float roll)
            {
                Rotation.x += pitch;
                Rotation.y += yaw;
                Rotation.z += roll;
                IsDirty = true;
            }

            void SetScale(float x, float y, float z)
            {
                Scale.x = x;
                Scale.y = y;
                Scale.z = z;
                IsDirty = true;
            }

            void SetScale(float scale)
            {
                Scale.x = scale;
                Scale.y = scale;
                Scale.z = scale;
                IsDirty = true;
            }

            void Toggle()
            {
                IsActive = !IsActive;
                IsDirty = true;
            }
        };
    }
}
