//
// File: Lifetime.hpp
// Description: ECS component that measures the lifetime of an entity
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Core/Timer.hpp"

namespace ThatEngine
{
    namespace ECS
    {
        struct Lifetime
        {
            Timer Timer;
        };
    }
}
