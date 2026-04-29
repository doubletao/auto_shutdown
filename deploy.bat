@echo off
echo ========================================
echo Deploy Qt libraries to output directory
echo ========================================

set QT_PATH=C:/Qt/5.15.2/msvc2019_64
set EXE_PATH=build\Release\auto_shutdown.exe

if not exist "%EXE_PATH%" (
    echo Error: Executable not found at %EXE_PATH%
    echo Please run build.bat first
    pause
    exit /b 1
)

echo Deploying...
%QT_PATH%/bin/windeployqt.exe --release --no-translations --no-system-d3d-compiler --no-compiler-runtime %EXE_PATH%

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Copying config.ini...
    copy /Y config.ini build\Release\ >nul

    echo.
    echo Deployment complete!
    echo You can now copy the entire build\Release folder to any machine
) else (
    echo Deployment failed
)

pause
