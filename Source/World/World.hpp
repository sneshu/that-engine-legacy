//
// File: World.hpp
// Description: Manages world, prepares renderable data for the renderer,
//              owns ECS registers and initializes entities, systems and components,
//              owns ECS systems and runs them
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Core/Window.hpp"
#include "Core/Timer.hpp"
#include "Core/JobManager.hpp"
#include "Core/StatsTracker.hpp"
#include "Renderer/ResourceManager.hpp"
#include "Renderer/Renderer.hpp"
#include "Types/ShaderTypes.hpp"
#include "Types/ECSTypes.hpp"
#include "World/System/SystemManager.hpp"
#include "World/Component/Transform.hpp"
#include "World/Component/Camera.hpp"

#include <entt/entt.hpp>

namespace ThatEngine
{
    class World
    {
        public:
        World() = default;

        void Init(Window* window, ResourceManager* resources, JobManager* jobs, Renderer* renderer, StatsTracker& statsTracker);
        void Update(Timestep time);
        void UpdateViewProjection(const ECS::Transform& transform, const ECS::Camera& camera);
        void Render();

        void SetActiveCamera(ECS::Entity entity);
        inline constexpr ECS::Entity GetActiveCamera() const { return m_ActiveCamera; }
        inline const GlobalData& GetGlobalData() const { return m_GlobalData; };

        private:
        void UpdateRenderableDatapack();
        void CreatePlayer();
        void CreateEnvironment();
        void CreateUI();
        
        private:
        Window* m_Window;
        ResourceManager* m_Resources;
        JobManager* m_Jobs;
        Renderer* m_Renderer;
        StatsTracker* m_StatsTracker;
        ECS::Registry m_Registry;
        ECS::SystemManager m_SystemManager;
        
        // Data
        RenderableDatapack m_RenderableDatapack;
        GlobalData m_GlobalData;
        Timer m_Timer;
        ECS::Entity m_ActiveCamera;

        // World-space entities
        ECS::Entity m_PlayerEntity;
        ECS::Entity m_WorldSpaceTextEntity;
        std::array<ECS::Entity, 250 * 250> m_EnvironmentEntities;

        // Screen-space entities (UI)
        ECS::Entity m_PerformanceMonitorEntity;
    };
}