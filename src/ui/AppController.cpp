#include "schedule/ui/AppController.h"

#include <cctype>
#include <exception>

#include "schedule/util/StrUtil.h"

namespace schedule {

AppController::AppController(ScheduleService& service, ConsoleView& view, std::string dataFilePath)
    : service_(service), view_(view), dataFilePath_(std::move(dataFilePath)) {}

void AppController::run() {
    today_   = Date::today();
    current_ = today_;

    while (running_) {
        render();

        std::string line;
        if (!view_.readLine("命令> ", line)) {
            running_ = false;           // 输入结束，当作退出
            break;
        }
        if (line.empty()) {
            continue;                   // 直接敲回车，什么都不做
        }

        const char cmd = static_cast<char>(std::tolower(static_cast<unsigned char>(line[0])));
        switch (cmd) {
            case 'a': handleAdd();    break;
            case 't': handleToggle(); break;
            case 'd': handleRemove(); break;
            case 'j': handleJump();   break;
            case 'n': shiftDay(1);    break;
            case 'p': shiftDay(-1);   break;
            case 'h':
                current_ = today_;
                view_.info("已回到今天：" + today_.toString());
                break;
            case 's': persist(false); break;
            case '?': view_.showHelp(); break;
            case 'q': running_ = false; break;
            default:
                view_.error(std::string("没有这个命令：") + line + "（输入 ? 查看帮助）");
                break;
        }
    }

    persist(true);
    view_.showGoodbye();
}

void AppController::render() const {
    view_.clear();
    if (firstRender_) {
        view_.showWelcome(dataFilePath_, today_);
        firstRender_ = false;
    }
    view_.showDay(service_, current_, today_);
}

void AppController::handleAdd() {
    view_.info("给 " + current_.toString() + " 添加一条计划（直接回车 = 取消）");

    std::string title;
    if (!view_.readLine("内容> ", title)) {
        running_ = false;
        return;
    }
    if (title.empty()) {
        view_.info("已取消。");
        return;
    }

    const int id = service_.addTask(current_, title);
    if (id < 0) {
        view_.error("内容不能为空。");
        return;
    }
    persist(true);
    view_.success("已添加：[" + std::to_string(id) + "] " + title);
}

void AppController::handleToggle() {
    const std::vector<const Task*> tasks = service_.tasksOn(current_);
    if (tasks.empty()) {
        view_.warn("这一天还没有计划，先按 a 添加一条吧。");
        return;
    }

    std::string line;
    if (!view_.readLine("要给第几条打勾 / 取消打勾？（直接回车 = 返回）> ", line)) {
        running_ = false;
        return;
    }
    if (line.empty()) {
        view_.info("已返回。");
        return;
    }

    int index = 0;
    if (!strutil::tryParseInt(line, index)) {
        view_.error("请输入数字序号，比如 2。");
        return;
    }
    if (index < 1 || index > static_cast<int>(tasks.size())) {
        view_.error("序号超出范围：这一天一共只有 " + std::to_string(tasks.size()) + " 条。");
        return;
    }

    const Task* task     = tasks[static_cast<std::size_t>(index - 1)];
    const bool  wasDone  = task->done();
    const std::string title = task->title();
    service_.toggleTask(task->id());
    persist(true);

    if (wasDone) {
        view_.success("已取消完成：" + title);
    } else {
        view_.success("已完成，划掉：" + title);
    }
}

void AppController::handleRemove() {
    const std::vector<const Task*> tasks = service_.tasksOn(current_);
    if (tasks.empty()) {
        view_.warn("这一天还没有计划。");
        return;
    }

    std::string line;
    if (!view_.readLine("要删除第几条？（直接回车 = 返回）> ", line)) {
        running_ = false;
        return;
    }
    if (line.empty()) {
        view_.info("已返回。");
        return;
    }

    int index = 0;
    if (!strutil::tryParseInt(line, index)) {
        view_.error("请输入数字序号，比如 2。");
        return;
    }
    if (index < 1 || index > static_cast<int>(tasks.size())) {
        view_.error("序号超出范围：这一天一共只有 " + std::to_string(tasks.size()) + " 条。");
        return;
    }

    const Task*       task  = tasks[static_cast<std::size_t>(index - 1)];
    const int         id    = task->id();
    const std::string title = task->title();
    service_.removeTask(id);
    persist(true);
    view_.success("已删除：" + title);
}

void AppController::handleJump() {
    Date target;
    if (!view_.readDate("跳到哪一天？（例 2026-09-15；也可以 t=今天 n=明天 y=昨天；回车=今天）> ",
                        target, today_)) {
        view_.error("日期看不懂，请按 2026-09-15 这样的格式输入。");
        return;
    }
    current_ = target;
    view_.info("已切换到 " + current_.toString() + " " + current_.weekdayName());
}

void AppController::shiftDay(int delta) {
    try {
        const Date next = current_.addDays(delta);
        if (next.year() < 1900 || next.year() > 9999) {
            view_.error("已经超出可显示的年份范围了。");
            return;
        }
        current_ = next;
    } catch (const std::exception& e) {
        view_.error(std::string("翻页失败：") + e.what());
    }
}

void AppController::persist(bool quiet) {
    std::string error;
    if (service_.save(error)) {
        if (!quiet) {
            view_.success("已保存到文件：数据不会丢。");
        }
    } else {
        view_.error("保存失败：" + error);
    }
}

}  // namespace schedule
