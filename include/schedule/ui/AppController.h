// AppController.h —— 总指挥：把"界面"和"业务"接在一起
//
// 【它负责什么】
//   一个循环：画画面 → 读命令 → 该谁干活就让谁干活 → 再画画面。
//   也就是"菜单逻辑"全在这里。它自己不算日期、不写文件，只负责调度。
//
// 【这样分层的好处】
//   想改成图形界面时，只要换掉 ConsoleView，
//   AppController 里的命令处理逻辑可以基本照搬。
#pragma once

#include <string>

#include "schedule/model/Date.h"
#include "schedule/service/ScheduleService.h"
#include "schedule/ui/ConsoleView.h"

namespace schedule {

class AppController {
public:
    AppController(ScheduleService& service, ConsoleView& view, std::string dataFilePath);

    // 进入主循环，直到用户输入 q
    void run();

private:
    void render() const;                        // 画当前这一屏
    void handleAdd();                           // a：添加
    void handleToggle();                        // t：打勾 / 取消
    void handleRemove();                        // d：删除
    void handleJump();                          // j：跳日期
    void shiftDay(int delta);                   // p / n：前后一天
    void persist(bool quiet);                   // 保存到文件

    ScheduleService& service_;
    ConsoleView&     view_;
    std::string      dataFilePath_;
    Date             today_;
    Date             current_;
    bool             running_     = true;
    mutable bool     firstRender_ = true;   // mutable：const 的 render() 里也要能改它
};

}  // namespace schedule
