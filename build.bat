@echo off
setlocal enabledelayedexpansion

:: Read and set variables from config file
for /f "usebackq tokens=1,* delims==" %%A in ("build_config.cfg") do (
    set "name=%%A"
    set "value=%%B"
    call set "!name!=!value!"
)

:: Build PCH if specified
if /I "!BUILD_PCH!"=="true" (
    call build_pch.bat
    echo.
)

:: Call script to build compiler flags and set defines
echo Building !APP_NAME! in !BUILD_MODE!_MODE:
call build_flags.bat

:: Set linker flags based on RELEASE_MODE in config
if /I "!BUILD_MODE!"=="RELEASE" (  
    set "LFLAGS=/link /OPT:REF /OPT:ICF /LTCG /INCREMENTAL:NO"
) else (
    set "LFLAGS=/link /DEBUG"
)

call "!VCVARS_PATH!"

:: Resource file
rc /fo "!OUTPUT_DIR_OBJ!\!APP_NAME!.res" "!RC_PATH!"

:: Set build variables
set INCLUDES=/I .\Source /I .\Source\Core /I %VULKAN_SDK%\Include /I .\Vendor\glm /I .\Vendor\spdlog\include /I .\Vendor\SPIRV-Reflect /I .\Vendor\stb /I .\Vendor\entt\src /I .\Vendor\tracy\public
set LINKS=!LFLAGS! /LIBPATH:%VULKAN_SDK%\Lib vulkan-1.lib user32.lib

:: Compile shaders
echo Compiling shaders to SPIR-V format:
for /r %%i in ("!SHADER_ASSETS!\*") do (
    if NOT "%%~xi" == ".spv" (
        echo Compiling %%i
        "%VULKAN_SDK%\Bin\glslc.exe" %%i -o %%i.spv
    )
)

:: Convert textures
echo. && echo Converting textures to DDS format:
for /r "%TEXTURE_ASSETS%" %%i in (*) do (
    if /I NOT "%%~xi" == ".dds" (
        echo Converting %%i
        "%TEXCONV_PATH%\texconv.exe" -f !TEXTURE_FORMAT! -m 1 -y -o %%~dpi %%i >nul 2>&1
    )
)

:: Add missing headers
echo. && echo Looking for missing headers in source:
for /r .\Source %%f in (*.cpp *.hpp) do (
    set "line="
    set /p line=<"%%f"

    echo !line! | findstr /b /c:"//" >nul
    if errorlevel 1 (
        echo Fixing missing source header: %%f
        call :AddHeader "%%f" "%%~xf" "%%~nxf"
    )
)

:: Add source files
echo. && echo Gathering source files:
SET "SOURCES="
for /r .\Source %%f in (*.cpp) do (
    if /I not "%%~nxf"=="PCH.cpp" (
        SET SOURCES=!SOURCES! "%%f"
    )
)

:: Recreate build directories
if not exist "!OUTPUT_DIR_OBJ!" mkdir "!OUTPUT_DIR_OBJ!"
if not exist "!OUTPUT_DIR_EXE!" mkdir "!OUTPUT_DIR_EXE!"

:: Build the application using PCH
for %%f in (!SOURCES!) do (
    cl !CFLAGS! !INCLUDES! !DEFINES! /Yu"Core/PCH.hpp" /Fp"!OUTPUT_DIR_OBJ!\PCH.pch" /Fo"!OUTPUT_DIR_OBJ!\%%~nxf.obj" %%f
)

:: Link everything
echo. && echo Linking stuff together:
set OBJS=
for %%f in (!SOURCES!) do (
    set OBJS=!OBJS! "!OUTPUT_DIR_OBJ!\%%~nxf.obj"
)
set OBJS=!OBJS! "!OUTPUT_DIR_OBJ!\PCH.obj" "!OUTPUT_DIR_OBJ!\spirv_reflect.obj" "!OUTPUT_DIR_OBJ!\stb_truetype.obj" "!OUTPUT_DIR_OBJ!\tracy.obj"

cl /Fe:"!OUTPUT_DIR_EXE!\!APP_NAME!.exe" !OBJS! !LINKS! "!OUTPUT_DIR_OBJ!\!APP_NAME!.res"
goto :eof

:: Functions
:: %1 = full path, %2 = extension, %3 = filename
:AddHeader
if /i "%~2"==".cpp" goto :AddCppHeader

:: .hpp file header
> "%~1.temp" echo %HEADER_EMPTY%
>> "%~1.temp" call echo %HEADER_FILENAME%%~3
>> "%~1.temp" echo %HEADER_DESCRIPTION%
>> "%~1.temp" echo %HEADER_EMPTY%
>> "%~1.temp" echo %HEADER_COPYRIGHT1%
>> "%~1.temp" echo %HEADER_COPYRIGHT2%
>> "%~1.temp" echo %HEADER_EMPTY%
>> "%~1.temp" echo.
>> "%~1.temp" type "%~1"
move /Y "%~1.temp" "%~1" >nul
goto :eof

:: .cpp file header
:AddCppHeader
> "%~1.temp" echo %HEADER_EMPTY%
>> "%~1.temp" call echo %HEADER_FILENAME%%~3
>> "%~1.temp" echo %HEADER_EMPTY%
>> "%~1.temp" echo %HEADER_COPYRIGHT1%
>> "%~1.temp" echo %HEADER_COPYRIGHT2%
>> "%~1.temp" echo %HEADER_EMPTY%
>> "%~1.temp" echo.
>> "%~1.temp" type "%~1"
move /Y "%~1.temp" "%~1" >nul
goto :eof