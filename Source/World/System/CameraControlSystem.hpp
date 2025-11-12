//
// File: FPCameraControlSystem.hpp
// Description: ECS system for first-person player camera
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Core/PCH.hpp"
#include "Core/Window.hpp"
#include "Core/Input.hpp"
#include "Types/ECSTypes.hpp"
#include "World/Component/Transform.hpp"
#include "World/Component/Camera.hpp"
#include "World/Component/Movement.hpp"
#include "World/Component/PlayerControl.hpp"

namespace ThatEngine
{
    namespace ECS
    {
        void CameraControlSystem(ECS::Registry& registry, Timestep deltaTime)
        {
            Window* window = registry.ctx().get<Window*>();

            if (!window->IsCursorLocked()) return;

            World* world = registry.ctx().get<World*>();
            Entity entity = world->GetActiveCamera();

            auto& transform = registry.get<ECS::Transform>(entity);
            auto& movement = registry.get<ECS::Movement>(entity);
            const auto& camera = registry.get<ECS::Camera>(entity);
            const auto& playerControl = registry.get<ECS::PlayerControl>(entity);

            glm::vec3 direction(0.0f);

            if (Input::IsKeyDown(playerControl.MoveForwardKey))
            {
                direction += transform.Forward;
            }

            if (Input::IsKeyDown(playerControl.MoveBackwardKey))
            {
                direction -= transform.Forward;
            }

            if (Input::IsKeyDown(playerControl.StrafeRightKey))
            {
                direction += transform.Right;
            }

            if (Input::IsKeyDown(playerControl.StrafeLeftKey))
            {
                direction -= transform.Right;
            }

            if (Input::IsKeyDown(playerControl.JumpKey))
            {
                direction += glm::vec3(0, 1.0f, 0);                    
            }

            if (Input::IsKeyDown(playerControl.DuckKey))
            {
                direction -= glm::vec3(0, 1.0f, 0);   
            }

            // Sprinting
            if (Input::KeyPressed(playerControl.SprintKey))
            {
                movement.CurrentMaxSpeed = movement.SprintMaxSpeed;
                movement.IsSprinting = true;
            }

            else if (Input::KeyReleased(playerControl.SprintKey))
            {
                movement.CurrentMaxSpeed = movement.DefaultMaxSpeed;
                movement.IsSprinting = false;
            }

            // Camera position
            if (direction.x != 0 || direction.y != 0 || direction.z != 0)
            {
                glm::vec3 translate = glm::normalize(direction) * movement.CurrentMaxSpeed * deltaTime.GetSeconds();
                transform.Translate(translate);
                movement.IsMoving = true;
            }

            else
            {
                movement.IsMoving = false;
            }

            // Camera rotation
            glm::vec2 mousePositionDelta = Input::GetMousePositionDelta();
            if (mousePositionDelta.x != 0 || mousePositionDelta.y != 0)
            {
                mousePositionDelta *= playerControl.MouseSensitivity;

                float pitch = glm::clamp<float>(transform.Rotation.x - mousePositionDelta.y, -89.99f, 89.99f);
                float yaw = transform.Rotation.y + mousePositionDelta.x;

                transform.SetRotation(pitch, yaw, 0.0);
            }   
        }
    }
}