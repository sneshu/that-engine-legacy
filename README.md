# That Engine
**That Engine** is a small game engine that I used to learn how to render 3D graphics using Vulkan API<br>
✅ Uses **[Vulkan API](https://www.khronos.org/vulkan/)** to provide cutting-edge graphics rendering.<br>
✅ Uses **[GLM](https://github.com/g-truc/glm)** for efficient math operations in 3D.<br>
✅ Uses **[spdlog](https://github.com/gabime/spdlog)** for header-only logging with modern C++ support.<br>
✅ Uses **[entt](https://github.com/skypjack/entt)** for fast and flexible Entity Component System.

## 1. 🖥️ Build Requirements
- **[Visual Studio 2022](https://visualstudio.microsoft.com/)**
- **[Vulkan SDK](https://vulkan.lunarg.com/)**
- **[Texconv](https://github.com/microsoft/DirectXTex/releases)**

## 2. 🛠️ Building That Engine
### How to build it?
- Run `git submodule update --init` to install all submodules
- Open `build_config.cfg` and change paths (especially `TEXCONV_PATH`)
- Simply hit `F5` via `VSCode` or run the `build.bat` script inside main directory
- `If you don't run it via VSCode, you must manually copy Assets folder to the .exe path`

### What does the script do?
- Builds precompiled headers if enabled in config
- Compiles all GLSL shaders to SPIR-V binary files
- Converts all textures inside assets directory to DDS format
- Adds missing documentation headers to source files
- Compiles the engine source code using MSVC

## 3. Controls
- Press `W`, `S`, `A`, `D`, `Left Alt`, `Space` to move the camera;
- Hold `Left Shift` to speed up the camera;
- Press `1` - `5` to switch between `COLOR`, `DEPTH`, `NORMALS`, `TRIANGLES` and `WIREFRAME` render modes;
