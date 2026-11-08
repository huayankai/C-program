// Task.h —— 一条计划
//
// 【这个类是干什么的】
//   一条计划 = 一个编号 + 属于哪一天 + 内容文字 + 有没有做完。
//   比如："3 号，2026-09-15 这一天，写高数作业，还没做完"。
//
// 【注意：第一版只有这一种 Task】
//   第二版会拆成"普通计划 / 每周重复计划 / 有截止日的计划"，
//   那时候 Task 会变成"抽象基类"，用继承 + 多态来实现。
//   现在先把最简单的一层做对、做稳。
#pragma once

#include <string>

#include "schedule/model/Date.h"

namespace schedule {

class Task {
public:
    Task() = default;

    // 构造一条计划：编号、哪一天、内容、是否已完成（默认没完成）
    Task(int id, const Date& date, const std::string& title, bool done = false);

    // ---- 只读访问 ----
    int                id() const { return id_; }
    const Date&        date() const { return date_; }
    const std::string& title() const { return title_; }
    bool               done() const { return done_; }

    // ---- 允许修改的部分 ----
    void setTitle(const std::string& title);
    void setDone(bool done) { done_ = done; }
    void toggleDone() { done_ = !done_; }        // 打勾 / 取消打勾

    // ---- 存盘用的"一行文本" ----
    // 格式：编号 <TAB> 日期 <TAB> 是否完成(1/0) <TAB> 内容
    std::string toLine() const;
    // 反过来：把一行文本解析成 Task，格式不对返回 false
    static bool fromLine(const std::string& line, Task& out);

private:
    int         id_ = 0;
    Date        date_;
    std::string title_;
    bool        done_ = false;
};

}  // namespace schedule
