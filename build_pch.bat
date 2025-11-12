@echo off
setlocal enabledelayedexpansion

:: Load config
for /f "usebackq tokens=1,* delims==" %%A in ("build_config.cfg") do (
    set "name=%%A"
    set "value=%%B"
    call set "!name!=!value!"
)

:: Call script to build compiler flags and set defines
echo Building PCH in !BUILD_MODE!_MODE:
call build_flags.bat

call "!VCVARS_PATH!"

:: Set include paths
set INCLUDES=/I .\Source /I .\Source\Core /I %VULKAN_SDK%\Include /I .\Vendor\glm /I .\Vendor\spdlog\include /I .\Vendor\SPIRV-Reflect /I .\Vendor\stb /I .\Vendor\entt\src /I .\Vendor\tracy\public

:: Create output dir if missing
if not exist "!OUTPUT_DIR_OBJ!" mkdir "!OUTPUT_DIR_OBJ!"

:: Compile headers
cl !CFLAGS! !INCLUDES! !DEFINES! /Fo"!OUTPUT_DIR_OBJ!\PCH.obj" /Fp"!OUTPUT_DIR_OBJ!\PCH.pch" /Yc"Core\PCH.hpp" "Source\Core\PCH.cpp"
cl !CFLAGS! !INCLUDES! !DEFINES! /Fo"!OUTPUT_DIR_OBJ!\spirv_reflect.obj" "Vendor\SPIRV-Reflect\spirv_reflect.cpp"
cl !CFLAGS! !INCLUDES! !DEFINES! /Fo"!OUTPUT_DIR_OBJ!\stb_truetype.obj" "Vendor\stb\stb_truetype.cpp"
cl !CFLAGS! !INCLUDES! !DEFINES! /Fo"!OUTPUT_DIR_OBJ!\tracy.obj" "Vendor\tracy\public\TracyClient.cpp"