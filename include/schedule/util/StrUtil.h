// StrUtil.h —— 字符串小工具
//
// 【这个类是干什么的】
//   把"去掉首尾空格""按分隔符切开""把文字转成整数"这类琐碎操作收在一起，
//   别的文件里就不用到处重复写这些小循环了。
#pragma once

#include <string>
#include <vector>

namespace schedule {
namespace strutil {

// 去掉首尾空白字符（空格、Tab、回车、换行）
std::string trim(const std::string& text);

// 按分隔符切成多段，空段保留（例如 "a\t\tb" 会得到 3 段）
std::vector<std::string> split(const std::string& text, char delimiter);

// 把字符串转成整数；成功返回 true，失败返回 false（不会让程序崩）
bool tryParseInt(const std::string& text, int& out);

// 把 from 全部替换成 to
std::string replaceAll(const std::string& text, const std::string& from, const std::string& to);

// 一个汉字在终端里大约占 2 格；这个函数用来估算"显示宽度"，
// 让菜单和列表能对齐（纯 ASCII 时等于字符个数）
std::size_t displayWidth(const std::string& text);

}  // namespace strutil
}  // namespace schedule
