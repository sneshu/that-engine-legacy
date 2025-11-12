//
// File: MemoryUtils.hpp
// Description: Memory related utils
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#include <psapi.h>
#endif

namespace ThatEngine
{
    namespace Utils
    {
        namespace Memory
        {
            inline float GetCurrentProcessMemoryUsage()
            {
                #ifdef PLATFORM_WINDOWS
                {
                    PROCESS_MEMORY_COUNTERS memCounters;
                    if (GetProcessMemoryInfo(GetCurrentProcess(), &memCounters, sizeof(memCounters)))
                    {
                        return static_cast<float>(memCounters.WorkingSetSize) / (1024.0f * 1024.0f);
                    }

                    return 0.0f;
                }
                
                #else
                {
                    return 0.0f;
                }
                #endif
            }
        }
    }
}