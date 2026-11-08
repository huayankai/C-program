#include "schedule/model/Task.h"

#include <cstdlib>
#include <vector>

#include "schedule/util/StrUtil.h"

namespace schedule {

Task::Task(int id, const Date& date, const std::string& title, bool done)
    : id_(id), date_(date), title_(title), done_(done) {}

void Task::setTitle(const std::string& title) {
    title_ = title;
}

std::string Task::toLine() const {
    // 内容里如果混进了 Tab 或换行，会把文件格式弄乱，
    // 所以存盘前统一换成空格。
    std::string safeTitle = title_;
    for (char& c : safeTitle) {
        if (c == '\t' || c == '\n' || c == '\r') {
            c = ' ';
        }
    }
    return std::to_string(id_) + "\t" + date_.toString() + "\t" +
           (done_ ? "1" : "0") + "\t" + safeTitle;
}

bool Task::fromLine(const std::string& line, Task& out) {
    const std::vector<std::string> fields = strutil::split(line, '\t');
    // 至少要有 3 个字段；第 4 个（内容）允许是空字符串
    if (fields.size() < 3) {
        return false;
    }

    int id = 0;
    if (!strutil::tryParseInt(fields[0], id) || id <= 0) {
        return false;
    }

    Date date;
    if (!Date::tryParse(fields[1], date)) {
        return false;
    }

    if (fields[2] != "0" && fields[2] != "1") {
        return false;
    }
    const bool done = (fields[2] == "1");

    // 内容里可能出现 Tab（理论上已经被清理过），把多出来的字段拼回去
    std::string title;
    for (std::size_t i = 3; i < fields.size(); ++i) {
        if (i > 3) {
            title += '\t';
        }
        title += fields[i];
    }

    out = Task(id, date, title, done);
    return true;
}

}  // namespace schedule
