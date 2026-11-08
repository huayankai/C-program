#include "schedule/ui/ConsoleView.h"

#include <cstdio>
#include <iostream>

#if defined(_WIN32)
#include <io.h>
#else
#include <unistd.h>
#endif

#include "schedule/util/Console.h"
#include "schedule/util/StrUtil.h"

namespace schedule {
namespace {

const char* kRule = "============================================================";

// 输出一行颜色文字后立刻复位颜色，避免"污染"后面的内容
void printColored(const char* color, const std::string& text) {
    std::cout << color << text << console::kReset << "\n";
}

// 当前查看的日期和"今天"是什么关系
std::string dayTag(const Date& current, const Date& today) {
    const int diff = daysBetween(today, current);
    if (diff == 0) {
        return std::string(console::kGreen) + "[今天]" + console::kReset;
    }
    if (diff == 1) {
        return std::string(console::kCyan) + "[明天]" + console::kReset;
    }
    if (diff == -1) {
        return std::string(console::kGray) + "[昨天]" + console::kReset;
    }
    if (diff > 1) {
        return std::string(console::kCyan) + "[还有 " + std::to_string(diff) + " 天]" +
               console::kReset;
    }
    return std::string(console::kGray) + "[已过去 " + std::to_string(-diff) + " 天]" +
           console::kReset;
}

// 一个 10 格的小进度条，比如 [####------]
std::string progressBar(int done, int total) {
    const int  slots    = 10;
    const int  filled   = (total <= 0) ? 0 : (done * slots) / total;
    std::string bar = "[";
    for (int i = 0; i < slots; ++i) {
        bar += (i < filled) ? '#' : '-';
    }
    bar += "]";
    return bar;
}

bool stdoutIsConsole() {
#if defined(_WIN32)
    return _isatty(_fileno(stdout)) != 0;
#else
    return isatty(fileno(stdout)) != 0;
#endif
}

}  // namespace

void ConsoleView::showWelcome(const std::string& dataFile, const Date& today) const {
    std::cout << "\n";
    printColored(console::kCyan, std::string(kRule));
    std::cout << "  " << console::kBold << "日程计划 v1.0" << console::kReset
              << "  —— 切日期、写计划、打勾划掉\n";
    printColored(console::kCyan, std::string(kRule));
    std::cout << "  今天是 " << today.toString() << " " << today.weekdayName() << "\n";
    std::cout << console::kGray << "  数据文件：" << dataFile << console::kReset << "\n";
    std::cout << console::kGray << "  输入 ? 看帮助，输入 q 保存并退出" << console::kReset << "\n\n";
}

void ConsoleView::showDay(const ScheduleService& service, const Date& current,
                          const Date& today) const {
    const std::vector<const Task*> tasks = service.tasksOn(current);

    int total = static_cast<int>(tasks.size());
    int done  = 0;
    for (const Task* task : tasks) {
        if (task->done()) {
            ++done;
        }
    }

    std::cout << "\n";
    printColored(console::kCyan, std::string(kRule));
    std::cout << "  " << console::kBold << "日程计划" << console::kReset << " · "
              << current.toString() << " " << current.weekdayName() << "   "
              << dayTag(current, today) << "\n";
    printColored(console::kCyan, std::string(kRule));

    if (current != today) {
        std::cout << console::kGray << "  今天：" << today.toString() << " "
                  << today.weekdayName() << "（按 h 可以回到今天）" << console::kReset << "\n";
    }

    if (total == 0) {
        std::cout << "\n  进度：0/0 " << progressBar(0, 0) << "\n";
    } else {
        std::cout << "\n  进度：" << done << "/" << total << " " << progressBar(done, total) << "  "
                  << console::kGreen << (done * 100 / total) << "%" << console::kReset << "\n";
    }

    std::cout << "------------------------------------------------------------\n";

    if (tasks.empty()) {
        printColored(console::kYellow, "   （这一天还没有计划，按 a 添加一条）");
    } else {
        int index = 0;
        for (const Task* task : tasks) {
            ++index;
            if (task->done()) {
                // 已完成：灰色 + 删除线（划掉）+ 后缀说明
                std::cout << "   " << console::kGreen << "[x]" << console::kReset << " " << index
                          << ". " << console::kGray << console::kStrike << task->title()
                          << console::kReset << console::kGray << "  ← 已完成" << console::kReset
                          << "\n";
            } else {
                std::cout << "   " << console::kYellow << "[ ]" << console::kReset << " " << index
                          << ". " << task->title() << "\n";
            }
        }
    }

    std::cout << "------------------------------------------------------------\n";
    std::cout << console::kGray
              << "  a 添加   t 打勾/取消   d 删除   j 跳转日期   p 前一天   n 后一天\n"
              << "  h 今天   s 保存        ? 帮助     q 退出\n"
              << console::kReset;
}

void ConsoleView::showHelp() const {
    std::cout << "\n";
    printColored(console::kCyan, "  【怎么用】");
    std::cout << "  输入一个字母 + 回车，就执行对应操作。\n\n";
    std::cout << "   a   添加计划：会问你要写什么内容\n";
    std::cout << "   t   打勾：输入列表里那条计划的序号，完成就划掉；再打一次取消\n";
    std::cout << "   d   删除：输入序号，删掉这一条\n";
    std::cout << "   j   跳转日期：输入 2026-09-15 这样的日期，也可以输入 t(今天) n(明天) y(昨天)\n";
    std::cout << "   p   看前一天        n   看后一天        h   回到今天\n";
    std::cout << "   s   立刻保存        ?   再显示这个帮助  q   保存并退出\n\n";
    std::cout << console::kGray
              << "   小提示：列表里的序号是「这一天里的第几条」，换日期后会重新从 1 开始数。"
              << console::kReset << "\n\n";
}

void ConsoleView::showGoodbye() const {
    std::cout << "\n";
    printColored(console::kGreen, "  已保存，再见。下次打开时计划还在。");
    std::cout << "\n";
}

bool ConsoleView::readLine(const std::string& prompt, std::string& out) const {
    std::cout << prompt << std::flush;
    std::string line;
    if (!std::getline(std::cin, line)) {
        out.clear();
        return false;                       // 输入结束（Ctrl+Z / Ctrl+D）
    }
    out = strutil::trim(line);
    return true;
}

bool ConsoleView::readInt(const std::string& prompt, int& out) const {
    std::string line;
    if (!readLine(prompt, line)) {
        return false;
    }
    return strutil::tryParseInt(line, out);
}

bool ConsoleView::readDate(const std::string& prompt, Date& out, const Date& today) const {
    std::string line;
    if (!readLine(prompt, line)) {
        return false;
    }
    if (line.empty()) {
        out = today;
        return true;
    }
    if (line == "t" || line == "T" || line == "today" || line == "今天") {
        out = today;
        return true;
    }
    if (line == "n" || line == "N" || line == "明天") {
        out = today.addDays(1);
        return true;
    }
    if (line == "y" || line == "Y" || line == "昨天") {
        out = today.addDays(-1);
        return true;
    }
    return Date::tryParse(line, out);
}

void ConsoleView::info(const std::string& text) const {
    std::cout << console::kCyan << "  " << text << console::kReset << "\n";
}

void ConsoleView::success(const std::string& text) const {
    std::cout << console::kGreen << "  " << text << console::kReset << "\n";
}

void ConsoleView::warn(const std::string& text) const {
    std::cout << console::kYellow << "  " << text << console::kReset << "\n";
}

void ConsoleView::error(const std::string& text) const {
    std::cout << console::kRed << "  " << text << console::kReset << "\n";
}

void ConsoleView::clear() const {
    if (stdoutIsConsole()) {
        std::cout << "\x1b[2J\x1b[H";       // 清屏并把光标移回左上角
    } else {
        std::cout << "\n";                  // 输出被重定向时不要清屏，否则日志会花
    }
    std::cout << std::flush;
}

}  // namespace schedule
