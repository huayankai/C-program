#include "schedule/storage/TextFileRepository.h"

#include <filesystem>
#include <fstream>
#include <system_error>

#include "schedule/util/StrUtil.h"

namespace fs = std::filesystem;

namespace schedule {
namespace {
const char* kHeaderLine = "#SCHEDULE v1";
}

TextFileRepository::TextFileRepository(std::string filePath) : filePath_(std::move(filePath)) {}

std::vector<Task> TextFileRepository::load() {
    std::vector<Task> tasks;
    skippedLines_ = 0;

    std::ifstream input(filePath_);
    if (!input.is_open()) {
        // 文件还不存在是正常情况（第一次运行），当作"一条计划都没有"
        return tasks;
    }

    std::string line;
    while (std::getline(input, line)) {
        const std::string trimmed = strutil::trim(line);
        if (trimmed.empty()) {
            continue;                       // 空行直接跳过
        }
        if (trimmed[0] == '#') {
            continue;                       // # 开头是注释 / 版本行
        }

        Task task;
        if (Task::fromLine(trimmed, task)) {
            tasks.push_back(task);
        } else {
            ++skippedLines_;                // 格式不对的行：跳过，不让整份数据报废
        }
    }
    return tasks;
}

bool TextFileRepository::save(const std::vector<Task>& tasks, std::string& error) {
    error.clear();

    std::error_code ec;
    const fs::path  target(filePath_);
    if (target.has_parent_path()) {
        fs::create_directories(target.parent_path(), ec);   // 目录不存在就建一个
    }

    const fs::path tempPath = fs::path(filePath_ + ".tmp");

    {
        std::ofstream output(tempPath, std::ios::trunc);
        if (!output.is_open()) {
            error = "无法写入文件：" + tempPath.string() + "（可能是目录只读或路径不存在）";
            return false;
        }
        output << kHeaderLine << "\n";
        for (const Task& task : tasks) {
            output << task.toLine() << "\n";
        }
        output.flush();
        if (!output.good()) {
            error = "写入文件时出错：" + tempPath.string() + "（磁盘空间够吗？）";
            return false;
        }
    }   // 作用域结束，文件自动关闭

    // 用新文件替换旧文件
    fs::rename(tempPath, target, ec);
    if (ec) {
        // 某些环境下目标存在时 rename 会失败，退一步：先删旧文件再改名
        fs::remove(target, ec);
        ec.clear();
        fs::rename(tempPath, target, ec);
        if (ec) {
            error = "保存失败，无法覆盖：" + target.string();
            return false;
        }
    }
    return true;
}

}  // namespace schedule
