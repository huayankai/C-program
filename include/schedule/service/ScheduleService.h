// ScheduleService.h —— 业务核心：所有"规则"都放在这里
//
// 【这一层负责什么】
//   往哪一天加计划、删掉哪一条、给哪一条打勾、某一天有哪些计划、
//   当天的完成进度是多少……这些"规则性"的事都归它管。
//
// 【这一层不负责什么】
//   不负责画界面（那是 ui 类的事），不负责读写文件（交给 repository）。
//   好处：以后换成图形界面，这里的规则一行都不用改。
#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "schedule/model/Task.h"
#include "schedule/storage/ITaskRepository.h"

namespace schedule {

class ScheduleService {
public:
    // 注意这里是"引用 &"：仓库对象由 main 创建并管理生命周期，
    // 服务只是用它，不负责删它。
    explicit ScheduleService(ITaskRepository& repository);

    // 从文件读入（程序启动时调一次）
    bool load(std::string& error);
    // 写回文件
    bool save(std::string& error);

    // 往某一天加一条计划，返回新计划的编号；内容为空时不加，返回 -1
    int addTask(const Date& date, const std::string& title);

    // 删除一条计划（按编号），成功返回 true
    bool removeTask(int id);

    // 给一条计划打勾 / 取消打勾，成功返回 true
    bool toggleTask(int id);

    // 按编号找一条计划，找不到返回 nullptr
    const Task* findById(int id) const;

    // 某一天的所有计划（按加入顺序），只给看、不给改
    std::vector<const Task*> tasksOn(const Date& date) const;

    // 某一天一共有几条 / 已经完成几条
    int countOn(const Date& date) const;
    int countDoneOn(const Date& date) const;

    // 全部计划（菜单里"查看全部"时用）
    const std::vector<Task>& allTasks() const { return tasks_; }

    std::size_t size() const { return tasks_.size(); }

private:
    int nextId();   // 分配一个新编号

    ITaskRepository&  repository_;
    std::vector<Task> tasks_;
    int               nextId_ = 1;
};

}  // namespace schedule
