# Auto Shutdown - Power Detection Auto Shutdown Tool

Resident process that monitors power status. Shows a countdown dialog when running on battery.
Runs in system tray with configurable timer intervals.

## Development Environment

- Qt 5.15.2 (msvc2019_64)
- Visual Studio 2019
- C++17

## Features

- Runs as a resident process in system tray
- Main window with log viewer (keeps last 50 entries)
- Configurable power detection interval via config.ini
- Configurable shutdown countdown via config.ini
- Tray menu:
  - **Show Window** - Show main window with log
  - **Check Now** - Trigger power detection immediately
  - **Open Config** - Open config.ini with Notepad
  - **Exit** - Exit the application
- Double-click tray icon to show main window
- Close window minimizes to tray
- Shows a top-most dialog when running on battery power
  - Displays current battery level
  - Cancel button to abort shutdown
  - Window stays always on top

## Configuration

`config.ini` in the same directory as the executable:

```ini
[General]
; Detection interval in seconds (default: 300)
DetectInterval=300

; Shutdown countdown in seconds after battery detected (default: 60)
ShutdownCountdown=60
```

## Build Instructions

### Quick Build

1. Open **"x64 Native Tools Command Prompt for VS 2019"** from Start Menu
2. Navigate to project: `cd D:\xiaolanpang\auto_shutdown`
3. Run: `build.bat`

The script will:
- Configure CMake
- Build Release version
- Automatically run `windeployqt` to copy Qt dependencies

After build, manually copy `config.ini` to `build\Release\` folder.

### Manual Build

```cmd
cd D:\xiaolanpang\auto_shutdown
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH="C:/Qt/5.15.2/msvc2019_64" -G "Visual Studio 16 2019" -A x64 ..
cmake --build . --config Release
C:\Qt\5.15.2\msvc2019_64\bin\windeployqt.exe --release --no-translations --no-compiler-runtime Release\auto_shutdown.exe
```

Then copy `config.ini` to `build\Release\` folder.

### Manual Deployment

If you only want to deploy (copy Qt libraries):
```cmd
deploy.bat
```

## Output

After successful build and setup, the `build\Release` folder contains:
- `auto_shutdown.exe` - Main executable
- `config.ini` - Configuration file (manual copy required)
- Qt5Core.dll, Qt5Gui.dll, Qt5Widgets.dll - Qt runtime libraries
- Platform plugins and other dependencies

The entire `Release` folder can be copied to any Windows machine.

## Deployment Suggestion

Add to Windows Startup:

1. Press `Win+R`, type `shell:startup`
2. Create a shortcut to `auto_shutdown.exe` in the opened folder

Or use Task Scheduler for more control:
- Trigger: At log on
- Action: Start a program
- Run with highest privileges (recommended for shutdown)

## Technical Notes

Core Windows APIs:
- `GetSystemPowerStatus()` - Get system power status
- `ExitWindowsEx()` - Perform system shutdown
- Qt 5.15.2 - GUI framework
- `QSystemTrayIcon` - System tray integration
- `QSettings` - INI configuration file
- `QPlainTextEdit` - Log viewer with 50-entry limit
