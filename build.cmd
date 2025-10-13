@echo off
setlocal enabledelayedexpansion

echo ========================================
echo MicroPomo Build Environment Setup
echo ========================================

REM Set project root to current script directory (relative)
set PROJECT_ROOT=%~dp0
set VCPKG_ROOT=%PROJECT_ROOT%vcpkg

echo Project Root: %PROJECT_ROOT%
echo VCPKG Root: %VCPKG_ROOT%

REM Check if git is available
git --version >nul 2>&1
IF !ERRORLEVEL! NEQ 0 (
    echo Error: Git is not installed or not in PATH
    echo Please install Git and make sure it's accessible from command line
    pause
    exit /b 1
)

REM Check if cmake is available  
cmake --version >nul 2>&1
IF !ERRORLEVEL! NEQ 0 (
    echo Error: CMake is not installed or not in PATH
    echo Please install CMake and make sure it's accessible from command line
    pause
    exit /b 1
)

REM Check if vcpkg exists, if not clone and bootstrap it
IF NOT EXIST "%VCPKG_ROOT%" (
    echo.
    echo [1/5] Cloning vcpkg...
    git clone https://github.com/microsoft/vcpkg.git vcpkg
    
    IF !ERRORLEVEL! NEQ 0 (
        echo Error: Failed to clone vcpkg repository
        echo Make sure you have internet connection and git access
        pause
        exit /b 1
    )
    
    echo [2/5] Bootstrapping vcpkg...
    cd /d vcpkg
    call bootstrap-vcpkg.bat
    
    IF !ERRORLEVEL! NEQ 0 (
        echo Error: Failed to bootstrap vcpkg
        pause
        exit /b 1
    )
    
    cd /d ..
) ELSE (
    echo [1/5] vcpkg already exists, skipping clone and bootstrap
)

REM Check if SDL2 is installed, if not install it
echo.
echo [2/5] Checking SDL2 installation...
IF NOT EXIST "vcpkg\installed\x64-windows\lib\sdl2.lib" (
    echo Installing SDL2...
    vcpkg\vcpkg.exe install sdl2:x64-windows
    
    IF !ERRORLEVEL! NEQ 0 (
        echo Error: Failed to install SDL2
        pause
        exit /b 1
    )
) ELSE (
    echo SDL2 already installed
)

REM Initialize git submodules (for LVGL)
echo.
echo [3/5] Initializing git submodules...
git submodule update --init --recursive

IF !ERRORLEVEL! NEQ 0 (
    echo Warning: Failed to initialize git submodules
    echo This might affect LVGL availability
)

REM Set environment variables for this session (all relative)
set CMAKE_TOOLCHAIN_FILE=vcpkg\scripts\buildsystems\vcpkg.cmake
set VCPKG_TARGET_TRIPLET=x64-windows
set SDL2_DIR=vcpkg\installed\x64-windows\share\sdl2

REM Generate build files
echo.
echo [4/5] Generating build files...
IF NOT EXIST "build" (
    cmake -B build -S . ^
        -DCMAKE_TOOLCHAIN_FILE="%CMAKE_TOOLCHAIN_FILE%" ^
        -DVCPKG_TARGET_TRIPLET=%VCPKG_TARGET_TRIPLET% ^
        -DSDL2_DIR="%SDL2_DIR%" ^
        -DCMAKE_BUILD_TYPE=Debug
        
    IF !ERRORLEVEL! NEQ 0 (
        echo Error: CMake configuration failed
        echo Make sure CMakeLists.txt exists and is properly configured
        pause
        exit /b 1
    )
) ELSE (
    echo Build folder exists, skipping generation...
)

REM Build the project
echo.
echo [5/5] Building project...
cmake --build build --config Debug -j8

IF !ERRORLEVEL! NEQ 0 (
    echo Error: Build failed!
    echo Check the error messages above for details
    pause
    exit /b 1
)

REM Find and run the executable
echo.
echo Build successful! Looking for executable...

REM Try different possible executable locations
IF EXIST "bin\Debug\main.exe" (
    echo Found executable at: bin\Debug\main.exe
    echo Running...
    "bin\Debug\main.exe"
) ELSE IF EXIST "bin\Debug\MicroPomo.exe" (
    echo Found executable at: bin\Debug\MicroPomo.exe
    echo Running...
    "bin\Debug\MicroPomo.exe"
) ELSE IF EXIST "bin\main.exe" (
    echo Found executable at: bin\main.exe
    echo Running...
    "bin\main.exe"
) ELSE IF EXIST "bin\MicroPomo.exe" (
    echo Found executable at: bin\MicroPomo.exe
    echo Running...
    "bin\MicroPomo.exe"
) ELSE (
    echo Warning: Could not find executable to run
    echo Build completed successfully, but executable location unknown
    echo Check the build folder manually
    pause
)

echo.
echo ========================================
echo Build process completed!
echo ========================================

REM Keep window open to see results
pause