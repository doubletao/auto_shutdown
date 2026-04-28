# Auto Shutdown - Power Detection Auto Shutdown Tool

Monitors power status, shows a countdown dialog when running on battery.
User can cancel shutdown within 60 seconds.

## Development Environment

- Qt 5.15.2 (msvc2019_64)
- Visual Studio 2019
- C++17

## Features

- Silent power status detection on startup
- AC power: no UI shown, exits immediately
- Battery power: shows a top-most dialog
  - 60 second countdown
  - Displays current battery level
  - Cancel button to abort shutdown
  - Window stays always on top

## Build Instructions

### Quick Build

1. Open **"x64 Native Tools Command Prompt for VS 2019"** from Start Menu
2. Navigate to project: `cd D:\xiaolanpang\auto_shutdown`
3. Run: `build.bat`

The script will:
- Configure CMake
- Build Release version
- Automatically run `windeployqt` to copy Qt dependencies

### Manual Build

```cmd
cd D:\xiaolanpang\auto_shutdown
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH="C:/Qt/5.15.2/msvc2019_64" -G "Visual Studio 16 2019" -A x64 ..
cmake --build . --config Release
C:\Qt\5.15.2\msvc2019_64\bin\windeployqt.exe --release --no-translations --no-compiler-runtime Release\auto_shutdown.exe
```

### Manual Deployment

If you only want to deploy (copy Qt libraries):
```cmd
deploy.bat
```

## Output

After successful build, the `build\Release` folder contains:
- `auto_shutdown.exe` - Main executable
- Qt5Core.dll, Qt5Gui.dll, Qt5Widgets.dll - Qt runtime libraries
- Platform plugins and other dependencies

The entire `Release` folder can be copied to any Windows machine.

## Deployment Suggestion

Add to Windows Task Scheduler:

1. Open "Task Scheduler"
2. Create Task
   - Trigger: At log on
   - Action: Start a program → select `auto_shutdown.exe`
   - Conditions: **UNCHECK** "Start the task only if the computer is on AC power"
   - Settings: Check "Run whether user is logged on or not"

## Technical Notes

Core Windows APIs:
- `GetSystemPowerStatus()` - Get system power status
- `ExitWindowsEx()` - Perform system shutdown
- Qt 5.15.2 - GUI framework
