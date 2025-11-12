//
// File: KeyCodes.hpp
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Core/Defines.hpp"

namespace ThatEngine
{
    enum class Key : uint32_t
    {
        // Printable characters
        Space       = 32,

        D0          = 48,
        D1          = 49,
        D2          = 50,
        D3          = 51,
        D4          = 52,
        D5          = 53,
        D6          = 54,
        D7          = 55,
        D8          = 56,
        D9          = 57,

        A           = 65,
        B           = 66,
        C           = 67,
        D           = 68,
        E           = 69,
        F           = 70,
        G           = 71,
        H           = 72,
        I           = 73,
        J           = 74,
        K           = 75,
        L           = 76,
        M           = 77,
        N           = 78,
        O           = 79,
        P           = 80,
        Q           = 81,
        R           = 82,
        S           = 83,
        T           = 84,
        U           = 85,
        V           = 86,
        W           = 87,
        X           = 88,
        Y           = 89,
        Z           = 90,

        Semicolon   = 186,
        Equal       = 187,
        Comma       = 188,
        Minus       = 189,
        Period      = 190,
        Slash       = 191,
        GraveAccent = 192,
        LeftBracket = 220,
        Backslash   = 221,
        RightBracket= 222,

        // Function keys
        Backspace   = 8,
        Tab         = 9,
        Enter       = 13,
        Pause       = 19,
        CapsLock    = 20,
        Escape      = 27,
        PageUp      = 33,
        PageDown    = 34,
        Home        = 36,
        ArrowLeft   = 37,
        ArrowUp     = 38,
        ArrowRight  = 39,
        ArrowDown   = 40,
        PrintScreen = 44,
        Insert      = 45,
        Delete      = 46,
        End         = 35,
        F1          = 112,
        F2          = 113,
        F3          = 114,
        F4          = 115,
        F5          = 116,
        F6          = 117,
        F7          = 118,
        F8          = 119,
        F9          = 120,
        F10         = 121,
        F11         = 122,
        F12         = 123,
        ScrollLock  = 145,

        // Modifier keys
        LeftShift   = 16,
        LeftControl = 17,
        RightControl= 18,
    };
}