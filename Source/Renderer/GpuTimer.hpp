//
// File: GpuTimer.hpp
// Description: 
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Renderer/GraphicsContext.hpp"

namespace ThatEngine
{
    class GpuTimer
    {
        public:
        GpuTimer() = default;

        void Init(VkDevice device)
        {
            m_Device = device;

            VkQueryPoolCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
            info.queryType = VK_QUERY_TYPE_TIMESTAMP;
            info.queryCount = m_QueryCount;

            vkCreateQueryPool(m_Device, &info, nullptr, &m_QueryPool);
        }       

        void Shutdown()
        {
            vkDestroyQueryPool(m_Device, m_QueryPool, nullptr);
            m_QueryPool = VK_NULL_HANDLE;
        }

        void StartTimestamp(VkCommandBuffer cmd)
        {
            vkCmdResetQueryPool(cmd, m_QueryPool, 0, m_QueryCount);
            vkCmdWriteTimestamp(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, m_QueryPool, 0);
        }

        void EndTimestamp(VkCommandBuffer cmd)
        {
            vkCmdWriteTimestamp(cmd, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, m_QueryPool, 1);
        }

        float GetElapsedTimeMs()
        {
            uint64_t timestamps[2];
            vkGetQueryPoolResults(m_Device, m_QueryPool, 0, 2, sizeof(timestamps), timestamps, sizeof(uint64_t), VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT);

            float elapsedTime = static_cast<float>(timestamps[1] - timestamps[0]) / 1e6f;

            return elapsedTime;
        }

        private:
        VkDevice m_Device;
        VkQueryPool m_QueryPool;
        uint32_t m_QueryCount = 2;
    };
}