// ConsoleView.h —— 负责"和用户打交道"：显示 + 读取输入
//
// 【职责边界】
//   它只做两件事：把内容画到屏幕上；把用户敲的字读进来并检查格式。
//   它不认识文件、不修改计划 —— 修改计划是 AppController 调 ScheduleService 干的事。
#pragma once

#include <string>

#include "schedule/model/Date.h"
#include "schedule/service/ScheduleService.h"

namespace schedule {

class ConsoleView {
public:
    // 画出某一天的画面
    void showDay(const ScheduleService& service, const Date& current, const Date& today) const;

    // 欢迎语 / 帮助 / 收尾
    void showWelcome(const std::string& dataFile, const Date& today) const;
    void showHelp() const;
    void showGoodbye() const;

    // ---- 读取输入（返回 false 表示用户按了 Ctrl+Z 之类的"输入结束"）----
    bool readLine(const std::string& prompt, std::string& out) const;
    bool readInt(const std::string& prompt, int& out) const;
    // 读取一个日期：直接回车=当天，t=今天，n=明天，y=昨天，也可以敲 2026-09-15
    bool readDate(const std::string& prompt, Date& out, const Date& today) const;

    // ---- 各种提示信息 ----
    void info(const std::string& text) const;
    void success(const std::string& text) const;
    void warn(const std::string& text) const;
    void error(const std::string& text) const;

    void clear() const;
};

}  // namespace schedule
