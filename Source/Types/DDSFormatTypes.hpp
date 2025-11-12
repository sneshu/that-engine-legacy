//
// File: DDSFormatTypes.hpp
// Description: Defines structs matching the DDS file format
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

namespace ThatEngine
{
    struct DDSPixelFormat
    {
        uint32_t Size;
        uint32_t Flags;
        uint32_t FourCC;
        uint32_t RGBBitCount;
        uint32_t RBitMask;
        uint32_t GBitMask;
        uint32_t BBitMask;
        uint32_t ABitMask;
    };

    struct DDSHeader
    {
        uint32_t Size;
        uint32_t Flags;
        uint32_t Height;
        uint32_t Width;
        uint32_t PitchOrLinearSize;
        uint32_t Depth;
        uint32_t MipMapCount;
        uint32_t Reserved1[11];
        DDSPixelFormat PixelFormat;
        uint32_t Caps1;
        uint32_t Caps2;
        uint32_t Caps3;
        uint32_t Caps4;
        uint32_t Reserved2;
    };

    struct DDSFile
    {
        uint8_t Magic[4];
        DDSHeader Header;
        uint8_t DataBegin;
    };
}