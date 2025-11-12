//
// File: FileReader.hpp
// Description: Provides utility functions to extract filenames,
//              reads binary files into application memory
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include <filesystem>
#include <fstream>
#include <cstdlib>

namespace ThatEngine
{
    class FileReader
    {
        public:
        static std::string GetFileName(const std::string& path)
        {
            std::filesystem::path filePath(path);
            if (!filePath.has_filename())
            {
                THAT_CORE_ERROR("Empty file name: {}", path);
                return "";
            }
            return filePath.filename().string();
        }

        template<typename T>
        static const T* ReadBinaryFile(const std::string& path, uint32_t& outFileSize)
        {
            std::ifstream file(path, std::ios::ate | std::ios::binary);
            if (!file.is_open())
            {
                THAT_CORE_ERROR("Failed to open file: '{}'.", path);
                return nullptr;
            }

            std::streamsize fileSize = file.tellg();

            if (fileSize < 0)
            {
                THAT_CORE_ERROR("Failed to determine size of file: '{}'.", path);
                return nullptr;
            }

            if (outFileSize % sizeof(T) != 0)
            {
                THAT_CORE_ERROR("File size is not a multiple of element size of type T: '{}'", path);
                return nullptr;
            }

            outFileSize = static_cast<uint32_t>(fileSize);
            uint32_t elementCount = outFileSize / sizeof(T);
            file.seekg(0, std::ios::beg);

            T* buffer = new T[elementCount];
            if (!file.read(reinterpret_cast<char*>(buffer), outFileSize))
            {
                THAT_CORE_ERROR("Failed to read file: '{}'.", path);
                delete [] buffer;
                return nullptr;
            }

            file.close();
            return buffer;
        }
    };
}