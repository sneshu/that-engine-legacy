//
// File: PCH.hpp
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <string>
#include <chrono>
#include <cstdint>
#include <memory>
#include <functional>
#include <unordered_map>
#include <queue>
#include <array>
#include <bitset>
#include <thread>
#include <mutex>
#include <future>
#include <condition_variable>
#include <atomic>

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <stb_truetype.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.hpp> 
#include <vulkan/vk_enum_string_helper.h>

#include <tracy/Tracy.hpp>

#include "Core/Defines.hpp"
#include "Core/Log.hpp"
#include "Core/Memory.hpp"
#include "Core/Timestep.hpp"
#include "Core/FileReader.hpp"