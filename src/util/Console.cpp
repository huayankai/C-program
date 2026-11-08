#include "schedule/util/Console.h"

#if defined(_WIN32)
#include <windows.h>
#endif

namespace schedule {
namespace console {
namespace {

#if defined(_WIN32)
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
UINT g_oldOutputCodePage = 0;
UINT g_oldInputCodePage  = 0;
bool g_inited            = false;
#endif

}  // namespace

void init() {
#if defined(_WIN32)
    // 1) 先记住原来的编码，退出时好还原
    g_oldOutputCodePage = GetConsoleOutputCP();
    g_oldInputCodePage  = GetConsoleCP();

    // 2) 输入输出都按 UTF-8 处理，中文才不会乱码
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // 3) 打开颜色支持（Windows 10 及以上支持）
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD  mode   = 0;
    if (handle != INVALID_HANDLE_VALUE && GetConsoleMode(handle, &mode)) {
        SetConsoleMode(handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
    g_inited = true;
#endif
}

void shutdown() {
#if defined(_WIN32)
    if (!g_inited) {
        return;
    }
    if (g_oldOutputCodePage != 0) {
        SetConsoleOutputCP(g_oldOutputCodePage);
    }
    if (g_oldInputCodePage != 0) {
        SetConsoleCP(g_oldInputCodePage);
    }
    g_inited = false;
#endif
}

}  // namespace console
}  // namespace schedule
