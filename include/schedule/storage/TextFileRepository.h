// TextFileRepository.h —— 用"文本文件"存计划
//
// 【文件长什么样】
//   data/tasks.txt：
//     #SCHEDULE v1                <- 第一行是格式说明，方便以后升级
//     1	2026-09-15	0	买牛奶      <- 编号 / 日期 / 是否完成 / 内容（用 Tab 隔开）
//     2	2026-09-15	1	写高数作业
//
// 【为什么存盘要先写 .tmp 再改名】
//   如果直接往原文件里写，写到一半断电/崩溃，原数据就毁了。
//   先写 tasks.txt.tmp，全部写完再改名覆盖 tasks.txt，
//   这样"要么是旧数据，要么是新数据"，不会出现半个文件。
#pragma once

#include <string>
#include <vector>

#include "schedule/storage/ITaskRepository.h"

namespace schedule {

class TextFileRepository : public ITaskRepository {
public:
    explicit TextFileRepository(std::string filePath);

    std::vector<Task> load() override;
    bool              save(const std::vector<Task>& tasks, std::string& error) override;

    const std::string& filePath() const { return filePath_; }

    // 读取时被跳过的坏行数量（调试时有用）
    int skippedLines() const { return skippedLines_; }

private:
    std::string filePath_;
    int         skippedLines_ = 0;
};

}  // namespace schedule
