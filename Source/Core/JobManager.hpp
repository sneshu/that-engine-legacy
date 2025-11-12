//
// File: JobManager.hpp
// Description: Manages a thread pool that executes submitted jobs asynchronously
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#include "Core/PCH.hpp"

#pragma once

namespace ThatEngine
{
    class JobManager
    {
        public:
        JobManager() = default;
        ~JobManager() = default;

        void Init()
        {
            m_ThreadCount = std::thread::hardware_concurrency();
            THAT_CORE_INFO("Job Manager: {} threads are available!", m_ThreadCount);

            m_Running = true;

            for (uint32_t i = 0; i < m_ThreadCount; i++)
            {
                m_Threads.emplace_back([&]()
                {
                    WorkerThread();
                });
            }
        }
        
        void Shutdown()
        {
            m_Running = false;
            m_Condition.notify_all();

            for (auto& thread : m_Threads)
            {
                if (!thread.joinable()) continue;
                
                thread.join();
            }

            m_Threads.clear();

            // Lock queue for safety
            {
                std::lock_guard<std::mutex> lock(m_QueueMutex);
                while(!m_JobQueue.empty())
                {
                    m_JobQueue.pop();
                }
            }
        }

        inline uint32_t GetThreadCount() const { return m_ThreadCount; }

        template<typename Function>
        auto Submit(Function&& task) -> std::future<decltype(task())>
        {
            using ReturnType = decltype(task());

            auto packagedTask = CreateShared<std::packaged_task<ReturnType()>>(std::forward<Function>(task));
            std::future<ReturnType> future = packagedTask->get_future();

            auto job = [packagedTask]() { (*packagedTask)(); };

            // Lock queue for safety
            {
                std::lock_guard<std::mutex> lock(m_QueueMutex);
                m_JobQueue.emplace(std::move(job));
            }

            m_Condition.notify_one();

            return future;
        }

        private:
        void WorkerThread()
        {
            while(m_Running)
            {
                std::function<void()> job;

                // Lock queue for safety
                {
                    std::unique_lock<std::mutex> lock(m_QueueMutex);
                    m_Condition.wait(lock, [&]() { return !m_Running || !m_JobQueue.empty(); });

                    if (!m_Running && m_JobQueue.empty()) return;

                    job = std::move(m_JobQueue.front());
                    m_JobQueue.pop();
                }

                job();
            }
        }

        private:
        uint32_t m_ThreadCount;
        
        std::queue<std::function<void()>> m_JobQueue;
        std::vector<std::thread> m_Threads;
        std::condition_variable m_Condition;
        std::mutex m_QueueMutex;
        std::atomic<bool> m_Running;
    };
}