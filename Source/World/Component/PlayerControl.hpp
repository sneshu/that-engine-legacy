//
// File: PlayerControl.hpp
// Description: ECS component storing player control variables
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Core/Keycodes.hpp"

#include <glm/glm.hpp>

namespace ThatEngine
{
    namespace ECS
    {
        struct PlayerControl
        {   
            float MouseSensitivity = 0.25f;
            
            Key MoveForwardKey = Key::W;
            Key MoveBackwardKey = Key::S;
            Key StrafeRightKey = Key::D;
            Key StrafeLeftKey = Key::A;
            Key JumpKey = Key::Space;
            Key DuckKey = Key::LeftControl;
            Key SprintKey = Key::LeftShift;
        };
    }
}
