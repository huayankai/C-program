#include "schedule/service/ScheduleService.h"

#include <algorithm>

#include "schedule/util/StrUtil.h"

namespace schedule {

ScheduleService::ScheduleService(ITaskRepository& repository) : repository_(repository) {}

bool ScheduleService::load(std::string& error) {
    error.clear();
    tasks_ = repository_.load();

    // 编号要接着已有的最大值往上走，否则重开程序会出现重复编号
    int maxId = 0;
    for (const Task& task : tasks_) {
        maxId = std::max(maxId, task.id());
    }
    nextId_ = maxId + 1;
    return true;
}

bool ScheduleService::save(std::string& error) {
    return repository_.save(tasks_, error);
}

int ScheduleService::addTask(const Date& date, const std::string& title) {
    const std::string trimmed = strutil::trim(title);
    if (trimmed.empty()) {
        return -1;                      // 空计划没有意义，不加
    }
    const int id = nextId();
    tasks_.emplace_back(id, date, trimmed, false);
    return id;
}

bool ScheduleService::removeTask(int id) {
    const auto it = std::find_if(tasks_.begin(), tasks_.end(),
                                 [id](const Task& task) { return task.id() == id; });
    if (it == tasks_.end()) {
        return false;
    }
    tasks_.erase(it);
    return true;
}

bool ScheduleService::toggleTask(int id) {
    const auto it = std::find_if(tasks_.begin(), tasks_.end(),
                                 [id](const Task& task) { return task.id() == id; });
    if (it == tasks_.end()) {
        return false;
    }
    it->toggleDone();
    return true;
}

const Task* ScheduleService::findById(int id) const {
    const auto it = std::find_if(tasks_.begin(), tasks_.end(),
                                 [id](const Task& task) { return task.id() == id; });
    return (it == tasks_.end()) ? nullptr : &(*it);
}

std::vector<const Task*> ScheduleService::tasksOn(const Date& date) const {
    std::vector<const Task*> result;
    for (const Task& task : tasks_) {
        if (task.date() == date) {
            result.push_back(&task);
        }
    }
    return result;
}

int ScheduleService::countOn(const Date& date) const {
    return static_cast<int>(tasksOn(date).size());
}

int ScheduleService::countDoneOn(const Date& date) const {
    int done = 0;
    for (const Task& task : tasks_) {
        if (task.date() == date && task.done()) {
            ++done;
        }
    }
    return done;
}

int ScheduleService::nextId() {
    return nextId_++;
}

}  // namespace schedule
