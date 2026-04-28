@echo off
echo ========================================
echo Auto Shutdown Build Script
echo ========================================

set QT_PATH=C:/Qt/5.15.2/msvc2019_64

if not exist "%QT_PATH%" (
    echo Error: Qt not found at %QT_PATH%
    pause
    exit /b 1
)

echo Qt path: %QT_PATH%
echo.

where cl.exe >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo Warning: MSVC compiler not found in PATH
    echo Please run vcvars64.bat first, or open
    echo "x64 Native Tools Command Prompt for VS 2019"
    echo.
)

if not exist build mkdir build
cd build

echo Configuring CMake...
cmake -DCMAKE_PREFIX_PATH="%QT_PATH%" -G "Visual Studio 16 2019" -A x64 ..
if %ERRORLEVEL% NEQ 0 (
    echo CMake configuration failed
    pause
    exit /b 1
)

echo.
echo Building...
cmake --build . --config Release
if %ERRORLEVEL% NEQ 0 (
    echo Build failed
    pause
    exit /b 1
)

echo.
echo Deploying Qt libraries...
%QT_PATH%/bin/windeployqt.exe --release --no-translations --no-system-d3d-compiler --no-compiler-runtime Release/auto_shutdown.exe
if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo Build and deployment successful!
    echo Output: build\Release\
    echo ========================================
) else (
    echo Deployment failed
)

pause
