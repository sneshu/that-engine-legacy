:: Load config
for /f "usebackq tokens=1,* delims==" %%A in ("build_config.cfg") do (
    set "name=%%A"
    set "value=%%B"
    call set "!name!=!value!"
)

:: Set flags and defines shared between debug and release mode 
set "CFLAGS=/c /WX /utf-8 /std:c++latest /nologo"
set "DEFINES=/D PLATFORM_!PLATFORM! /D !BUILD_MODE!" 

:: Set compiler flags based on build mode
if /I "!BUILD_MODE!"=="RELEASE" (
    set "CFLAGS=!CFLAGS! /O2 /Ot /GL /EHsc"
) else (
    set "CFLAGS=!CFLAGS! /Zi /EHsc"
)

:: Validation layers define
if /I "!USE_VULKAN_VALIDATION_LAYERS!"=="true" (
    echo USE_VULKAN_VALIDATION_LAYERS enabled
    set "DEFINES=!DEFINES! /D VULKAN_VALIDATION_LAYERS"
)

:: Tracy define
if /I "!USE_TRACY!"=="true" (
    echo USE_TRACY enabled
    set "DEFINES=!DEFINES! /D TRACY_ENABLE"
)

endlocal & set "CFLAGS=%CFLAGS%" & set "DEFINES=%DEFINES%"