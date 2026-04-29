# Install as Windows System Service

Runs in background without user login. Logs to `Documents\auto_shutdown\log.log`.

## Prerequisites

1. Download NSSM from https://nssm.cc/download
2. Extract nssm.exe (e.g., `C:\Tools\nssm.exe`)
3. Build the project with `build.bat`
4. Verify `build\Release\config.ini` exists

## Installation Steps

1. **Open Command Prompt as Administrator**

2. **Install service:**
```cmd
C:\Tools\nssm.exe install AutoShutdown
```

3. **Configure in NSSM dialog:**
   - **Path:** `D:\xiaolanpang\auto_shutdown\build\Release\auto_shutdown.exe`
   - **Startup directory:** `D:\xiaolanpang\auto_shutdown\build\Release`
   - **Arguments:** `--service`
   - **Service name:** `AutoShutdown`

4. **Log on tab:**
   - Select **"Local System account"**

5. **Recovery tab:**
   - First/Second/Subsequent failures: **Restart the service**
   - Restart after: **1 minute**

6. Click **"Install service"**

## Start Service

```cmd
C:\Tools\nssm.exe start AutoShutdown
```

## Verify Service

Check log file:
```
C:\ProgramData\auto_shutdown\log.log
```

Or: `services.msc` → Find "AutoShutdown" → Status should be "Running"

## Tray Application (Optional)

If you also want the tray UI when logged in:
1. Win+R → `shell:startup`
2. Create shortcut to `auto_shutdown.exe` (NO `--service` argument)

Both service and tray app run independently with separate mutexes.

## Uninstall Service

```cmd
C:\Tools\nssm.exe stop AutoShutdown
C:\Tools\nssm.exe remove AutoShutdown confirm
```

## Notes

- **Service mode** (`--service`): No UI, no dialog, shutdown silently when on battery
- **Log location**: `C:\ProgramData\auto_shutdown\log.log`
- Log is cleared on service startup, appended during runtime
- Log file is opened/closed on each write (no file locking)
