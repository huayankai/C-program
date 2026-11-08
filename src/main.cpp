// main.cpp —— 程序的入口
//
// 【这里故意写得很短】
//   它只做四件事：初始化控制台 → 创建"存文件的仓库" → 创建"业务服务" →
//   交给总指挥 AppController 跑起来。
//
//   业务规则、界面细节都不在 main 里，以后改功能基本不用动这个文件。
#include <exception>
#include <iostream>
#include <string>

#include "schedule/model/Date.h"
#include "schedule/service/ScheduleService.h"
#include "schedule/storage/TextFileRepository.h"
#include "schedule/ui/AppController.h"
#include "schedule/ui/ConsoleView.h"
#include "schedule/util/Console.h"

int main(int argc, char** argv) {
    // 1) 让控制台能正确显示中文、能显示颜色
    schedule::console::init();

    // 2) 数据文件的位置，默认 data/tasks.txt，可以用 --data 指定别的路径
    std::string dataFile = "data/tasks.txt";
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if ((arg == "--data" || arg == "-d") && i + 1 < argc) {
            dataFile = argv[++i];
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "用法：ScheduleApp [--data 数据文件路径]\n";
            schedule::console::shutdown();
            return 0;
        }
    }

    try {
        // 3) 一层一层搭起来：仓库 -> 服务 -> 界面 -> 总指挥
        schedule::TextFileRepository repository(dataFile);
        schedule::ScheduleService    service(repository);

        std::string error;
        service.load(error);
        if (!error.empty()) {
            std::cout << "读取数据时出问题：" << error << "\n";
        }
        if (repository.skippedLines() > 0) {
            std::cout << "注意：数据文件里有 " << repository.skippedLines()
                      << " 行格式不对，已跳过（原文件没有被改动）。\n";
        }

        schedule::ConsoleView   view;
        schedule::AppController controller(service, view, dataFile);
        controller.run();
    } catch (const std::exception& e) {
        std::cout << "\n程序出错了：" << e.what() << "\n";
        schedule::console::shutdown();
        return 1;
    }

    // 4) 退出前把控制台编码还原，否则这个窗口后面的中文会乱码
    schedule::console::shutdown();
    return 0;
}
