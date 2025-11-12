//
// File: Defines.hpp
// Description: Contains common utility macros for event binding, array size calculation, memory size conversions etc.
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#define BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define SIZE_KB(x) ((uint32_t)x * 1024)
#define SIZE_MB(x) (SIZE_KB(x) * 1024)
#define SIZE_GB(x) (SIZE_MB(x) * 1024)
#define INVALID_UINT32_ID UINT32_MAX