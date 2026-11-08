@echo off
setlocal
cd /d "%~dp0"

echo ============================================
echo   编译 日程计划 v1.0
echo ============================================
echo.

where g++ >nul 2>nul
if errorlevel 1 (
    echo [错误] 没找到 g++ 编译器。
    echo        请先看 README.md 的「第一步：装好编译器」，
    echo        装好 MinGW-w64 之后重新双击本文件。
    echo.
    pause
    exit /b 1
)

if not exist build mkdir build

echo 正在编译，请稍等十几秒...
echo.
g++ -std=c++17 -O2 -Wall -Wextra -Iinclude ^
    src\main.cpp ^
    src\model\Date.cpp ^
    src\model\Task.cpp ^
    src\storage\TextFileRepository.cpp ^
    src\service\ScheduleService.cpp ^
    src\ui\ConsoleView.cpp ^
    src\ui\AppController.cpp ^
    src\util\StrUtil.cpp ^
    src\util\Console.cpp ^
    -o build\ScheduleApp.exe

if errorlevel 1 (
    echo.
    echo [失败] 编译没通过。把上面的报错信息截图或复制下来。
    echo.
    pause
    exit /b 1
)

echo.
echo [成功] 已经生成 build\ScheduleApp.exe
echo        双击 run.bat 就能开始用。
echo.
pause
