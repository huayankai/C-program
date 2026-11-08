// Console.h —— 让 Windows 控制台能正确显示中文、能显示颜色
//
// 【为什么需要它】
//   Windows 控制台默认编码是 GBK，而我们的源码和文件都是 UTF-8，
//   不设置的话中文会变成一堆乱码。
//   另外，要显示颜色需要打开"虚拟终端"开关（ANSI 转义序列）。
//
// 【为什么要 shutdown】
//   控制台编码是"整个窗口"共用的。程序退出时如果不还原，
//   命令行窗口会一直停在 UTF-8 状态，后面跑 .bat 里的中文就乱码了。
#pragma once

namespace schedule {
namespace console {

// 程序启动时调用一次：把输入输出编码设成 UTF-8，并打开颜色支持
void init();

// 程序退出前调用一次：把控制台编码还原成进来时的样子
void shutdown();

// 常用的颜色 / 样式控制码（\x1b 是转义字符 ESC）
inline constexpr const char* kReset   = "\x1b[0m";
inline constexpr const char* kBold    = "\x1b[1m";
inline constexpr const char* kDim     = "\x1b[2m";
inline constexpr const char* kStrike  = "\x1b[9m";   // 删除线（划掉）
inline constexpr const char* kRed     = "\x1b[31m";
inline constexpr const char* kGreen   = "\x1b[32m";
inline constexpr const char* kYellow  = "\x1b[33m";
inline constexpr const char* kCyan    = "\x1b[36m";
inline constexpr const char* kGray    = "\x1b[90m";

}  // namespace console
}  // namespace schedule
