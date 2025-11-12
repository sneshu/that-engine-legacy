//
// File: EntryPoint.cpp
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#include "Core/PCH.hpp"
#include "Core/Application.hpp"

int main(int argc, char** argv)
{
    ThatEngine::Application& app = ThatEngine::Application::Get();
    
    if (!app.Init())
    {
        return -1;
    }

    app.Run();

    return 0;
}
