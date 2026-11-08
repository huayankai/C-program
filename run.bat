@echo off
setlocal
cd /d "%~dp0"

if not exist build\ScheduleApp.exe (
    echo 还没有编译过，先帮你编译一次...
    call build.bat
    if not exist build\ScheduleApp.exe (
        echo 编译没成功，先解决编译问题再运行。
        pause
        exit /b 1
    )
)

build\ScheduleApp.exe
echo.
pause
