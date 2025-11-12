//
// File: UpdateWorldSpaceTransformSystem.hpp
// Description: ECS system that recalculates transform vectors and model matrix in world space             
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Core/PCH.hpp"
#include "Core/JobManager.hpp"
#include "Types/ECSTypes.hpp"
#include "World/Component/Transform.hpp"
#include "Utils/GeometryUtils.hpp"

namespace ThatEngine
{
    namespace ECS
    {
        void UpdateWorldSpaceTransformSystem(ECS::Registry& registry, Timestep deltaTime)
        {
            auto view = registry.view<ECS::Transform, ECS::WorldSpace>();
            auto* world = registry.ctx().get<World*>();
            auto* jobs = registry.ctx().get<JobManager*>();
            auto globalData = world->GetGlobalData();
            ECS::Entity activeCamera = world->GetActiveCamera();

            Utils::Geometry::Plane frustumPlanes[6];
            Utils::Geometry::ExtractFrustumPlanes(globalData.PerspectiveViewProjection, frustumPlanes);
            
            // Prepare for jobs
            std::vector<ECS::Entity> entities(view.begin(), view.end());
            std::vector<std::future<void>> futures;
            const uint32_t threadCount = jobs->GetThreadCount();
            const uint32_t entityCount = view.size_hint();
            const uint32_t batchSize = (entityCount + threadCount - 1) / threadCount;

            for (uint32_t i = 0; i < threadCount; i++)
            {
                uint32_t start = i * batchSize;
                uint32_t end = glm::min(start + batchSize, entityCount);

                if (start >= end) break;

                futures.push_back(jobs->Submit([start, end, &entities, &registry, activeCamera, &frustumPlanes]()
                {
                    for (uint32_t i = start; i < end; i++)
                    {
                        auto& transform = registry.get<ECS::Transform>(entities[i]);

                        // Frustum culling
                        transform.IsVisible = transform.IsActive && Utils::Geometry::IsSphereInsideFrustum(transform.Position, transform.BoundingRadius, frustumPlanes);

                        if ((!transform.IsDirty || !transform.IsVisible) && entities[i] != activeCamera) continue;

                        // Recalculate directional vectors
                        float pitch = transform.Rotation.x;
                        float yaw = transform.Rotation.y;

                        glm::vec3 forward = 
                        {
                            cos(glm::radians(pitch)) * sin(glm::radians(yaw)),
                            sin(glm::radians(pitch)),
                            cos(glm::radians(pitch)) * cos(glm::radians(yaw))
                        };
                        
                        transform.Forward = glm::normalize(forward);
                        transform.Right = glm::normalize(glm::cross(transform.Forward, glm::vec3(0.0f, -1.0f, 0.0f)));
                        transform.Up = glm::normalize(glm::cross(transform.Forward, transform.Right));
                        
                        // Recalculate model matrix
                        glm::mat4 translation = glm::translate(glm::mat4(1.0f), transform.Position);
                        glm::mat4 scale = glm::scale(glm::mat4(1.0f), transform.Scale);
                        glm::mat4 rotation = glm::mat4(1.0f);
                        rotation[0] = glm::vec4(transform.Right, 0.0f);
                        rotation[1] = glm::vec4(transform.Up, 0.0f);
                        rotation[2] = glm::vec4(transform.Forward, 0.0f);
                        
                        transform.BoundingRadius = glm::length(transform.Scale) * 0.5f;
                        transform.Model = translation * rotation * scale;
                    
                        // Mark as clean
                        transform.IsDirty = false;
                    }
                }));
            }

            // Wait for all jobs to finish
            for (auto& future : futures) 
            {
                future.get();
            }
        }
    }
}