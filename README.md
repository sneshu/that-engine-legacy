# That Engine
**That Engine** is a high-performance voxel game engine created for `Voxadion` game.<br>
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
- Simply run the `build.bat` script inside main directory
- `If you don't run it via VSCode, you must manually copy Assets folder to the .exe path`

### What does the script do?
- Builds precompiled headers if enabled in config
- Compiles all GLSL shaders to SPIR-V binary files
- Converts all textures inside assets directory to DDS format
- Adds missing documentation headers to source files
- Compiles the engine source code using MSVC
