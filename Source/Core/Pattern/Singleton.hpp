//
// File: Singleton.hpp
// Description: Generic singleton template
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

namespace ThatEngine
{
    template<typename T>
    class Singleton
    {
        public:
        Singleton() = default;
        ~Singleton() = default;
        Singleton(const Singleton&) = delete;
        Singleton& operator=(const Singleton&) = delete;
        static T& Get()
        {
            static T instance;
            return instance;
        } 
    };
}