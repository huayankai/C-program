@echo off
setlocal
cd /d "%~dp0"

where g++ >nul 2>nul
if errorlevel 1 (
    echo [错误] 没找到 g++ 编译器，请先看 README.md 的「第一步：装好编译器」。
    pause
    exit /b 1
)

if not exist build mkdir build

echo 正在编译测试程序...
g++ -std=c++17 -Wall -Wextra -Iinclude ^
    tests\test_main.cpp ^
    src\model\Date.cpp ^
    src\model\Task.cpp ^
    src\storage\TextFileRepository.cpp ^
    src\service\ScheduleService.cpp ^
    src\util\StrUtil.cpp ^
    -o build\tests.exe

if errorlevel 1 (
    echo.
    echo [失败] 测试程序编译没通过。
    pause
    exit /b 1
)

echo.
build\tests.exe
echo.
pause
