// ITaskRepository.h —— "存计划的地方"的抽象接口
//
// 【为什么要有个接口】
//   现在的存法是把计划写成文本文件（TextFileRepository）。
//   以后如果改成存数据库、存 JSON，只要再写一个新类实现这个接口，
//   上层的 ScheduleService 一行都不用改。这就是"面向接口编程"。
//
//   纯虚函数 = 0，表示"这个函数这里不实现，由子类负责"。
//   接口类必须有一个虚析构函数，否则通过父类指针删除子类对象会出错。
#pragma once

#include <string>
#include <vector>

#include "schedule/model/Task.h"

namespace schedule {

class ITaskRepository {
public:
    virtual ~ITaskRepository() = default;

    // 读出所有计划
    virtual std::vector<Task> load() = 0;

    // 把所有计划写回存储；失败时把原因写进 error 并返回 false
    virtual bool save(const std::vector<Task>& tasks, std::string& error) = 0;
};

}  // namespace schedule
