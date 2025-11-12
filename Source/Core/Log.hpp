//
// File: Log.hpp
// Description: Implements logging system for the engine and client using spdlog library
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Core/Defines.hpp"
#include "Core/Memory.hpp"
#include "Core/Pattern/Singleton.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <iomanip>

namespace ThatEngine
{
    class Log : public Singleton<Log>
    {
        friend class Singleton<Log>;

        public:
        void Init()
        {
            std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            std::tm localTime = *std::localtime(&currentTime);
            std::stringstream timeStream;
            timeStream << std::put_time(&localTime, "%Y-%m-%d_%H-%M-%S");
        
            const std::string outputPattern = "[%T] [%n] [%^%l%$] %v";
            auto consoleSink = CreateShared<spdlog::sinks::wincolor_stdout_sink_mt>();
            consoleSink->set_pattern(outputPattern);
            
            const std::string fileName = "./Logs/" + timeStream.str() + ".log";
            auto fileSink = CreateShared<spdlog::sinks::basic_file_sink_mt>(fileName, true);
            fileSink->set_pattern(outputPattern);

            spdlog::sinks_init_list sinkList { consoleSink, fileSink };

            m_CoreLogger = CreateShared<spdlog::logger>("Engine", sinkList);
            m_CoreLogger->set_level(spdlog::level::trace);
            spdlog::register_logger(m_CoreLogger);

            m_ClientLogger = CreateShared<spdlog::logger>("Client", sinkList);
            m_ClientLogger->set_level(spdlog::level::trace);
            spdlog::register_logger(m_ClientLogger);
        }

        inline Shared<spdlog::logger>& GetCoreLogger() { return m_CoreLogger; }
        inline Shared<spdlog::logger>& GetClientLogger() { return m_ClientLogger; }

        protected:
        Log() = default;

        Shared<spdlog::logger> m_CoreLogger;
        Shared<spdlog::logger> m_ClientLogger;
    };

    #define THAT_CORE_TRACE(...)    ::ThatEngine::Log::Get().GetCoreLogger()->trace(__VA_ARGS__)
    #define THAT_CORE_INFO(...)     ::ThatEngine::Log::Get().GetCoreLogger()->info(__VA_ARGS__)
    #define THAT_CORE_WARN(...)     ::ThatEngine::Log::Get().GetCoreLogger()->warn(__VA_ARGS__)
    #define THAT_CORE_ERROR(...)    ::ThatEngine::Log::Get().GetCoreLogger()->error(__VA_ARGS__)
    #define THAT_CORE_FATAL(...)    ::ThatEngine::Log::Get().GetCoreLogger()->critical(__VA_ARGS__)

    #define THAT_TRACE(...)         ::ThatEngine::Log::Get().GetClientLogger()->trace(__VA_ARGS__)
    #define THAT_INFO(...)          ::ThatEngine::Log::Get().GetClientLogger()->info(__VA_ARGS__)
    #define THAT_WARN(...)          ::ThatEngine::Log::Get().GetClientLogger()->warn(__VA_ARGS__)
    #define THAT_ERROR(...)         ::ThatEngine::Log::Get().GetClientLogger()->error(__VA_ARGS__)
    #define THAT_FATAL(...)         ::ThatEngine::Log::Get().GetClientLogger()->critical(__VA_ARGS__)

    #ifdef DEBUG
    #define THAT_CORE_ASSERT(x, message, ...)       \
    {                                               \
        if (!(x))                                   \
        {                                           \
            THAT_CORE_ERROR(message, __VA_ARGS__);  \
            __debugbreak();                         \
        }                                           \
    }
    #else
    #define THAT_CORE_ASSERT(x, message, ...)
    #endif
}

