#include "schedule/model/Date.h"

#include <cstdio>
#include <ctime>
#include <stdexcept>

namespace schedule {
namespace {

// 把"年月日"换算成"距 1970-01-01 的天数"。
// 这是 Howard Hinnant（C++ 标准库日期库的作者）给出的经典算法，
// 几行代码就正确处理了闰年和世纪规则，不需要 if 一大堆。
long long daysFromCivil(int year, int month, int day) {
    year -= (month <= 2) ? 1 : 0;
    const int era = (year >= 0 ? year : year - 399) / 400;
    const int yoe = year - era * 400;                                   // 0..399
    const int mp  = (month > 2) ? (month - 3) : (month + 9);            // 3 月当作一年的开始
    const int doy = (153 * mp + 2) / 5 + day - 1;                       // 0..365
    const int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;              // 0..146096
    return static_cast<long long>(era) * 146097LL + doe - 719468LL;
}

// 反过来：天数 -> 年月日
void civilFromDays(long long z, int& year, int& month, int& day) {
    z += 719468;
    const long long era  = (z >= 0 ? z : z - 146096) / 146097;
    const int       doe  = static_cast<int>(z - era * 146097);          // 0..146096
    const int       yoe  = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    year = yoe + static_cast<int>(era) * 400;
    const int doy = doe - (365 * yoe + yoe / 4 - yoe / 100);            // 0..365
    const int mp  = (5 * doy + 2) / 153;                                // 0..11
    day   = doy - (153 * mp + 2) / 5 + 1;                               // 1..31
    month = (mp < 10) ? (mp + 3) : (mp - 9);                            // 1..12
    year += (month <= 2) ? 1 : 0;
}

long long toDays(const Date& d) {
    return daysFromCivil(d.year(), d.month(), d.day());
}

}  // namespace

Date::Date(int year, int month, int day) {
    if (year < 1900 || year > 9999) {
        throw std::invalid_argument("年份必须在 1900 ~ 9999 之间");
    }
    if (month < 1 || month > 12) {
        throw std::invalid_argument("月份必须在 1 ~ 12 之间");
    }
    if (day < 1 || day > daysInMonth(year, month)) {
        throw std::invalid_argument("这个月的天数不对（比如 2 月只有 28/29 天）");
    }
    year_  = year;
    month_ = month;
    day_   = day;
}

bool Date::isLeapYear(int year) {
    // 四年一闰，百年不闰，四百年再闰
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int Date::daysInMonth(int year, int month) {
    static const int kDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month < 1 || month > 12) {
        return 0;
    }
    if (month == 2 && isLeapYear(year)) {
        return 29;
    }
    return kDays[month - 1];
}

Date Date::today() {
    const std::time_t now = std::time(nullptr);
    std::tm local{};
#if defined(_WIN32)
    localtime_s(&local, &now);      // Windows 的安全版本
#else
    localtime_r(&now, &local);      // Linux / macOS
#endif
    return Date(local.tm_year + 1900, local.tm_mon + 1, local.tm_mday);
}

bool Date::tryParse(const std::string& text, Date& out) {
    // 先去掉首尾空格
    std::size_t begin = text.find_first_not_of(" \t\r\n");
    std::size_t end   = text.find_last_not_of(" \t\r\n");
    if (begin == std::string::npos) {
        return false;
    }
    const std::string trimmed = text.substr(begin, end - begin + 1);

    // 支持 "2026-09-15" 和 "2026/9/15" 两种写法
    int parts[3] = {0, 0, 0};
    int index    = 0;
    int value    = 0;
    bool hasDigit = false;
    for (std::size_t i = 0; i <= trimmed.size(); ++i) {
        const char c = (i < trimmed.size()) ? trimmed[i] : '-';
        if (c >= '0' && c <= '9') {
            value    = value * 10 + (c - '0');
            hasDigit = true;
        } else if (c == '-' || c == '/') {
            if (!hasDigit || index >= 3) {
                return false;
            }
            parts[index++] = value;
            value          = 0;
            hasDigit       = false;
        } else {
            return false;
        }
    }
    if (index != 3) {
        return false;
    }
    if (parts[0] < 1900 || parts[0] > 9999) {
        return false;
    }
    if (parts[1] < 1 || parts[1] > 12) {
        return false;
    }
    if (parts[2] < 1 || parts[2] > daysInMonth(parts[0], parts[1])) {
        return false;
    }
    out = Date(parts[0], parts[1], parts[2]);
    return true;
}

Date Date::addDays(int days) const {
    int year  = 0;
    int month = 0;
    int day   = 0;
    civilFromDays(toDays(*this) + days, year, month, day);
    return Date(year, month, day);
}

int Date::weekday() const {
    // 1970-01-01 是星期四，所以 +4 之后再取模
    const long long raw = (toDays(*this) + 4LL) % 7LL;
    return static_cast<int>(raw < 0 ? raw + 7LL : raw);
}

std::string Date::weekdayName() const {
    static const char* kNames[] = {"星期日", "星期一", "星期二", "星期三",
                                   "星期四", "星期五", "星期六"};
    return kNames[weekday()];
}

std::string Date::toString() const {
    char buffer[16] = {0};
    std::snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d", year_, month_, day_);
    return std::string(buffer);
}

bool Date::operator==(const Date& other) const {
    return year_ == other.year_ && month_ == other.month_ && day_ == other.day_;
}

bool Date::operator!=(const Date& other) const {
    return !(*this == other);
}

bool Date::operator<(const Date& other) const {
    return toDays(*this) < toDays(other);
}

int daysBetween(const Date& from, const Date& to) {
    const long long diff = toDays(to) - toDays(from);
    return static_cast<int>(diff);
}

}  // namespace schedule
