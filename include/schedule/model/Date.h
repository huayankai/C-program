// Date.h —— 日期类
//
// 【这个类是干什么的】
//   一个"日期"就是一个年月日，比如 2026-09-15。
//   我们要能判断它是星期几、能往前/往后翻几天、能比较大小。
//   这些逻辑统统写在 Date 里面，别人用的时候只管调函数，不用自己算。
//
// 【为什么不用现成的 tm 结构】
//   C 语言的 tm + mktime 会牵扯到时区、夏令时，很容易算错。
//   自己用"距 1970-01-01 的天数"来算，干净、可测试，也正好练封装。
#pragma once

#include <string>

namespace schedule {

class Date {
public:
    // 默认构造：1970-01-01（只是为了方便，比如数组里先放个空值）
    Date() = default;

    // 构造：年月日非法（比如 2026-02-30）会抛 std::invalid_argument
    Date(int year, int month, int day);

    // ---- 静态工具函数（属于"日期"这个概念，但不依赖某个具体日期）----
    static Date today();                                    // 取今天的日期
    static bool isLeapYear(int year);                       // 是不是闰年
    static int  daysInMonth(int year, int month);           // 这个月有几天
    static bool tryParse(const std::string& text, Date& out);  // 把 "2026-9-15" 解析成 Date

    // ---- 读取内部数据（只读，外部改不了，这就是"封装"）----
    int year()  const { return year_; }
    int month() const { return month_; }
    int day()   const { return day_; }

    // ---- 日期运算 ----
    Date addDays(int days) const;      // 往后推 n 天（n 可以是负数，就是往前）
    int  weekday() const;              // 星期几：0=周日，1=周一 … 6=周六
    std::string weekdayName() const;   // "星期一" 这样的中文名

    std::string toString() const;      // "2026-09-15"

    // ---- 让日期可以直接用 == != < 比较（运算符重载）----
    bool operator==(const Date& other) const;
    bool operator!=(const Date& other) const;
    bool operator<(const Date& other) const;

private:
    // 数据成员一律私有：外部只能通过上面的函数访问
    int year_  = 1970;
    int month_ = 1;
    int day_   = 1;
};

// 两个日期相差多少天：返回 to - from（to 在后就是正数）
int daysBetween(const Date& from, const Date& to);

}  // namespace schedule
