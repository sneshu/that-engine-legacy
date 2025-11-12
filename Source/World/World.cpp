//
// File: World.cpp
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#include "Core/PCH.hpp"
#include "Core/KeyCodes.hpp"
#include "World/World.hpp"
#include "Core/StatsTracker.hpp"
#include "Types/ECSTypes.hpp"
#include "Types/RendererTypes.hpp"
#include "Utils/ColorUtils.hpp"

// Components
#include "World/Component/Transform.hpp"
#include "World/Component/WorldSpace.hpp"
#include "World/Component/ScreenSpace.hpp"
#include "World/Component/Camera.hpp"
#include "World/Component/Movement.hpp"
#include "World/Component/PlayerControl.hpp"
#include "World/Component/Mesh.hpp"
#include "World/Component/Text.hpp"
#include "World/Component/Wave.hpp"
#include "World/Component/PerformanceMonitor.hpp"
// Systems
#include "World/System/UpdateWorldSpaceTransformSystem.hpp"
#include "World/System/UpdateScreenSpaceTransformSystem.hpp"
#include "World/System/UpdateCameraSystem.hpp"
#include "World/System/CameraControlSystem.hpp"
#include "World/System/UpdatePerformanceMonitorSystem.hpp" 
#include "World/System/WaveSystem.hpp"
#include "World/System/RotateTextSystem.hpp"

#include <entt/entt.hpp>

namespace ThatEngine
{
    void World::Init(Window* window, ResourceManager* resources, JobManager* jobs, Renderer* renderer, StatsTracker& statsTracker)
    {
        m_Window = window;
        m_Resources = resources;
        m_Jobs = jobs;
        m_Renderer = renderer;
        m_StatsTracker = &statsTracker;

        // ECS registry context
        m_Registry.ctx().emplace<Window*>(m_Window);
        m_Registry.ctx().emplace<Renderer*>(m_Renderer);
        m_Registry.ctx().emplace<ResourceManager*>(m_Resources);
        m_Registry.ctx().emplace<JobManager*>(m_Jobs);
        m_Registry.ctx().emplace<StatsTracker*>(m_StatsTracker);
        m_Registry.ctx().emplace<World*>(this);

        // Register systems
        m_SystemManager.AddSystem(ECS::UpdateScreenSpaceTransformSystem);
        m_SystemManager.AddSystem(ECS::UpdateWorldSpaceTransformSystem);
        m_SystemManager.AddSystem(ECS::CameraControlSystem);
        m_SystemManager.AddSystem(ECS::UpdateCameraSystem);
        m_SystemManager.AddSystem(ECS::UpdatePerformanceMonitorSystem);
        m_SystemManager.AddSystem(ECS::WaveSystem);
        m_SystemManager.AddSystem(ECS::RotateTextSystem);

        // Create entities
        CreatePlayer();
        CreateEnvironment();
        CreateUI();
        
        // Start the timer
        m_Timer.Reset();
    }

    void World::Update(Timestep deltaTime)
    {
        m_SystemManager.Update(m_Registry, deltaTime);
        
        m_GlobalData.LightColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.2f);
        m_GlobalData.LightDirection = glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f));
        m_GlobalData.FogParams = glm::vec2(0.9f, 1.0f);
        m_GlobalData.SkyColor = glm::vec4(0.19f, 0.29f, 0.52f, 1.0f);
        m_GlobalData.RenderMode = static_cast<uint32_t>(m_Renderer->GetRenderMode());
        m_GlobalData.Time = m_Timer.GetElapsedTime().GetSeconds();

        m_Renderer->UploadGlobalData(m_GlobalData);
    }

    void World::UpdateViewProjection(const ECS::Transform& transform, const ECS::Camera& camera)
    {
        glm::mat4 view = glm::lookAt(transform.Position, transform.Position - transform.Forward, transform.Up);
        
        m_GlobalData.ScreenSize = glm::vec4(m_Window->GetInnerWidth(), m_Window->GetInnerHeight(), camera.NearPlane, camera.FarPlane);
        m_GlobalData.PerspectiveViewProjection = glm::perspectiveFovLH(glm::radians(camera.Fov), m_GlobalData.ScreenSize.x, m_GlobalData.ScreenSize.y, camera.NearPlane, camera.FarPlane) * view;    
        m_GlobalData.OrthographicViewProjection = glm::orthoLH(0.0f, m_GlobalData.ScreenSize.x, 0.0f, m_GlobalData.ScreenSize.y, -1.0f, 1.0f);
    }

    void World::UpdateRenderableDatapack()
    {
        m_RenderableDatapack.Clear();

        // Clear color
        m_RenderableDatapack.ClearColor = Utils::Color::ToLinear(m_GlobalData.SkyColor);

        // Mesh instances
        {
            auto view = m_Registry.view<ECS::Transform, ECS::WorldSpace, ECS::Mesh>();

            view.each([&](auto entity, const auto& transform, const auto& mesh)
            {
                if (!transform.IsVisible || !transform.IsActive) return;

                const MeshInstance meshInstance { transform.Model };
                m_RenderableDatapack.MeshInstanceBatches[mesh.Type].Instances.emplace_back(meshInstance);
            });
        }

        // Text glyph instances 
        {
            constexpr glm::vec3 shadowOffset = glm::vec3(0.0f, -1.0f, 0.0f);
            auto view = m_Registry.view<ECS::Transform, ECS::Text>();

            view.each([&](auto entity, const auto& transform, const auto& text)
            {
                if (!transform.IsVisible || !transform.IsActive) return;
                
                bool isScreenSpace = m_Registry.any_of<ECS::ScreenSpace>(entity);
                auto& batchInstances = (isScreenSpace ? m_RenderableDatapack.ScreenSpaceGlyphInstanceBatches[text.Font] : m_RenderableDatapack.WorldSpaceGlyphInstanceBatches[text.Font]).Instances;
                
                const Shared<FontAtlasData>& fontAtlasData = m_Resources->GetFontManager().GetFontAtlasData(text.Font);
                const float atlasFontSize = fontAtlasData->FontSize;
                const float fontScale = text.FontSize / atlasFontSize;
                const float fontSizeScaled = fontScale * (isScreenSpace ? 1.0f : 1.0f / atlasFontSize);
                glm::mat4 glyphScale = glm::scale(glm::mat4(1.0f), glm::vec3(fontScale));
                float glyphOffset = 0;

                for (uint32_t i = 0; i < text.Content.size(); i++)
                {
                    uint32_t glyph = static_cast<uint32_t>(text.Content[i]);

                    if (glyph == static_cast<uint32_t>(Key::Space))
                    {
                        glyphOffset += 32.0f * fontSizeScaled;
                        continue;
                    }

                    const GlyphData& glyphData = fontAtlasData->GetGlyphData(glyph);
                    float alignX = (glyphData.Rect.z * 0.5f + glyphData.Offset.x) * fontSizeScaled;
                    float alignY = -(glyphData.Rect.w * 0.5f + glyphData.Offset.y) * fontSizeScaled;

                    glm::vec3 glyphPosition = glm::vec3(glyphOffset + alignX, alignY, 0.0f);
                    glm::mat4 glyphModel = glm::translate(transform.Model, glyphPosition) * glyphScale;
                    GlyphInstance glyphInstance { glyphModel, glyphData.Rect, text.Color };
                    batchInstances.emplace_back(glyphInstance);
                    
                    // Drop shadow in screen space
                    if (isScreenSpace)
                    {
                        glm::vec3 shadowPosition = glyphPosition + shadowOffset;
                        glm::mat4 shadowModel = glm::translate(transform.Model, shadowPosition) * glyphScale;
                        GlyphInstance shadowInstance { shadowModel, glyphData.Rect, Utils::Color::Black };
                        batchInstances.emplace_back(shadowInstance);
                    }

                    glyphOffset += glyphData.AdvanceX * fontSizeScaled;
                }
            });
        }
    }

    void World::Render()
    {
        UpdateRenderableDatapack();
        m_Renderer->Render(m_RenderableDatapack);
    }

    void World::SetActiveCamera(ECS::Entity entity)
    {
        m_ActiveCamera = entity; 
    }

    void World::CreatePlayer()
    {
        m_PlayerEntity = m_Registry.create();
        m_Registry.emplace<ECS::Transform>(m_PlayerEntity, glm::vec3(0.0, 10.0, 0.0));
        m_Registry.emplace<ECS::WorldSpace>(m_PlayerEntity);
        m_Registry.emplace<ECS::Camera>(m_PlayerEntity);
        m_Registry.emplace<ECS::Movement>(m_PlayerEntity);
        m_Registry.emplace<ECS::PlayerControl>(m_PlayerEntity);

        SetActiveCamera(m_PlayerEntity);
    }

    void World::CreateEnvironment()
    {
        // Wavy cube entities
        {
            const uint32_t entityCount = static_cast<uint32_t>(m_EnvironmentEntities.size());
            const uint32_t entityRowCount = static_cast<uint32_t>(glm::sqrt(entityCount));
            const float entitySpacing = 0.5f; 
            const float entityOffset = entityRowCount * entitySpacing * 0.5f;
            
            for (uint32_t i = 0; i < entityCount; i++)
            {
                m_EnvironmentEntities[i] = m_Registry.create();

                const float positionX = (i % entityRowCount) * entitySpacing - entityOffset;
                const float positionY = 0.0f;
                const float positionZ = (i / entityRowCount) * entitySpacing - entityOffset;
                
                const float frequency = 1.0f;
                const float frequencyOffset = 0.05f * static_cast<float>((positionX / entitySpacing) + (positionZ / entitySpacing));
                const float maxHeight = 2.5f + 0.5f * static_cast<float>(glm::sin((frequencyOffset)));
                const glm::vec3 position = glm::vec3(positionX, positionY, positionZ);
                const glm::vec3 rotation = glm::vec3(0.0f);
                const glm::vec3 scale(entitySpacing);

                m_Registry.emplace<ECS::Transform>(m_EnvironmentEntities[i], position, rotation, scale);
                m_Registry.emplace<ECS::WorldSpace>(m_EnvironmentEntities[i]);
                m_Registry.emplace<ECS::Mesh>(m_EnvironmentEntities[i], MeshAssetType::Cube);
                m_Registry.emplace<ECS::Wave>(m_EnvironmentEntities[i], frequency, frequencyOffset, maxHeight);
            }
        }

        // World space text
        {
            m_WorldSpaceTextEntity = m_Registry.create();
            m_Registry.emplace<ECS::Transform>(m_WorldSpaceTextEntity, glm::vec3(0.0f, 10.0f, 10.0f));
            m_Registry.emplace<ECS::WorldSpace>(m_WorldSpaceTextEntity);
            m_Registry.emplace<ECS::Text>(m_WorldSpaceTextEntity, FontAssetType::Default, 144.0f, Utils::Color::Yellow, "Here is some world space text!");
        }
    } 

    void World::CreateUI()
    {
        // Performance monitor
        {
            ECS::Entity renderModeEntity = m_Registry.create();
            m_Registry.emplace<ECS::Transform>(renderModeEntity, glm::vec3(5.0f, 65.0f, 0.0f));
            m_Registry.emplace<ECS::ScreenSpace>(renderModeEntity);
            m_Registry.emplace<ECS::Text>(renderModeEntity, FontAssetType::Default, 14.0f, Utils::Color::Yellow);

            ECS::Entity cpuTimeEntity = m_Registry.create();
            m_Registry.emplace<ECS::Transform>(cpuTimeEntity, glm::vec3(5.0f, 50.0f, 0.0f));
            m_Registry.emplace<ECS::ScreenSpace>(cpuTimeEntity);
            m_Registry.emplace<ECS::Text>(cpuTimeEntity, FontAssetType::Default, 14.0f, Utils::Color::Yellow);

            ECS::Entity gpuTimeEntity = m_Registry.create();
            m_Registry.emplace<ECS::Transform>(gpuTimeEntity, glm::vec3(5.0f, 35.0f, 0.0f));
            m_Registry.emplace<ECS::ScreenSpace>(gpuTimeEntity);
            m_Registry.emplace<ECS::Text>(gpuTimeEntity, FontAssetType::Default, 14.0f, Utils::Color::Yellow);

            ECS::Entity ramUsageEntity = m_Registry.create();
            m_Registry.emplace<ECS::Transform>(ramUsageEntity, glm::vec3(5.0f, 20.0f, 0.0f));
            m_Registry.emplace<ECS::ScreenSpace>(ramUsageEntity);
            m_Registry.emplace<ECS::Text>(ramUsageEntity, FontAssetType::Default, 14.0f, Utils::Color::Yellow);

            ECS::Entity fpsCounterEntity = m_Registry.create();
            m_Registry.emplace<ECS::Transform>(fpsCounterEntity, glm::vec3(5.0f, 5.0f, 0.0f));
            m_Registry.emplace<ECS::ScreenSpace>(fpsCounterEntity);
            m_Registry.emplace<ECS::Text>(fpsCounterEntity, FontAssetType::Default, 14.0f, Utils::Color::Yellow);

            m_PerformanceMonitorEntity = m_Registry.create();
            m_Registry.emplace<ECS::PerformanceMonitor>(m_PerformanceMonitorEntity, renderModeEntity, cpuTimeEntity, gpuTimeEntity, ramUsageEntity, fpsCounterEntity);
            m_Registry.emplace<ECS::Lifetime>(m_PerformanceMonitorEntity);
        }   
    }
}