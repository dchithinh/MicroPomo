@echo off

IF NOT EXIST "build" (
    echo Generating build files...
    cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=D:/learn/lvgl/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows -DSDL2_DIR=D:/learn/lvgl/vcpkg/installed/x64-windows/share/sdl2
) ELSE (
    echo Build folder exists, skipping generation...
)

echo Building project...
cmake --build build -j8

IF %ERRORLEVEL% EQU 0 (
    echo Build successful, running executable...
    .\bin\Debug\main.exe
) ELSE (
    echo Build failed!
    exit /b %ERRORLEVEL%
)