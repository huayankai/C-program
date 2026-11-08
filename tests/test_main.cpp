// test_main.cpp —— 单元测试（不需要第三方测试框架，自己写几个断言宏就够了）
//
// 【怎么运行】
//   双击 build_tests.bat，或者在命令行里：
//     g++ -std=c++17 -Iinclude tests\test_main.cpp src\model\Date.cpp ... -o build\tests.exe
//     build\tests.exe
//
// 【为什么值得写】
//   日期算法（闰年、跨月、跨年）特别容易错，靠手输命令测试很难覆盖。
//   有了这些测试，改代码之后跑一遍就知道有没有弄坏别的东西。
#include <cstdio>
#include <filesystem>
#include <string>
#include <vector>

#include "schedule/model/Date.h"
#include "schedule/model/Task.h"
#include "schedule/service/ScheduleService.h"
#include "schedule/storage/TextFileRepository.h"
#include "schedule/util/StrUtil.h"

using namespace schedule;

namespace {

int g_total  = 0;
int g_failed = 0;
int g_cases  = 0;

void beginCase(const char* name) {
    ++g_cases;
    std::printf("[用例 %d] %s\n", g_cases, name);
}

#define CHECK(expr)                                                       \
    do {                                                                  \
        ++g_total;                                                        \
        if (!(expr)) {                                                    \
            ++g_failed;                                                   \
            std::printf("    失败 -> 第 %d 行: %s\n", __LINE__, #expr);   \
        }                                                                 \
    } while (false)

template <typename A, typename B>
void checkEqualImpl(int line, const char* text, const A& left, const B& right) {
    ++g_total;
    if (!(left == right)) {
        ++g_failed;
        std::printf("    失败 -> 第 %d 行: %s （左右不相等）\n", line, text);
    }
}

#define CHECK_EQ(a, b) checkEqualImpl(__LINE__, #a " == " #b, (a), (b))

// ---------- 1. 日期基础 ----------
void testDateBasics() {
    beginCase("闰年判断");
    CHECK(Date::isLeapYear(2024));
    CHECK(Date::isLeapYear(2000));
    CHECK(!Date::isLeapYear(1900));      // 百年不闰
    CHECK(!Date::isLeapYear(2023));

    beginCase("每月天数");
    CHECK_EQ(Date::daysInMonth(2024, 2), 29);
    CHECK_EQ(Date::daysInMonth(2023, 2), 28);
    CHECK_EQ(Date::daysInMonth(2026, 4), 30);
    CHECK_EQ(Date::daysInMonth(2026, 12), 31);

    beginCase("非法日期要报错");
    bool threw = false;
    try {
        Date bad(2023, 2, 29);           // 2023 年没有 2 月 29 日
        (void)bad;
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    CHECK(threw);

    threw = false;
    try {
        Date bad(2026, 13, 1);
        (void)bad;
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    CHECK(threw);
}

// ---------- 2. 日期运算 ----------
void testDateMath() {
    beginCase("加点天数（跨闰日 / 跨月 / 跨年）");
    CHECK_EQ(Date(2024, 2, 28).addDays(1).toString(), std::string("2024-02-29"));
    CHECK_EQ(Date(2024, 2, 28).addDays(2).toString(), std::string("2024-03-01"));
    CHECK_EQ(Date(2024, 12, 31).addDays(1).toString(), std::string("2025-01-01"));
    CHECK_EQ(Date(2026, 1, 1).addDays(-1).toString(), std::string("2025-12-31"));
    CHECK_EQ(Date(2024, 3, 1).addDays(-1).toString(), std::string("2024-02-29"));

    beginCase("星期几");
    CHECK_EQ(Date(2026, 1, 1).weekday(), 4);    // 2026-01-01 是星期四
    CHECK_EQ(Date(2000, 1, 1).weekday(), 6);    // 2000-01-01 是星期六
    CHECK_EQ(Date(2026, 1, 1).weekdayName(), std::string("星期四"));
    // 任何一个日期加 7 天，星期几不变
    CHECK_EQ(Date(2026, 9, 15).addDays(7).weekday(), Date(2026, 9, 15).weekday());

    beginCase("两个日期相差几天");
    CHECK_EQ(daysBetween(Date(2026, 1, 1), Date(2026, 1, 31)), 30);
    CHECK_EQ(daysBetween(Date(2024, 2, 28), Date(2024, 3, 1)), 2);
    CHECK_EQ(daysBetween(Date(2026, 1, 31), Date(2026, 1, 1)), -30);

    beginCase("文字与日期互转");
    CHECK_EQ(Date(2026, 9, 5).toString(), std::string("2026-09-05"));
    Date parsed;
    CHECK(Date::tryParse("2026-09-15", parsed));
    CHECK_EQ(parsed.toString(), std::string("2026-09-15"));
    CHECK(Date::tryParse("2026/9/5", parsed));      // 斜杠也认
    CHECK_EQ(parsed.toString(), std::string("2026-09-05"));
    CHECK(!Date::tryParse("2026-02-30", parsed));   // 2 月没有 30 日
    CHECK(!Date::tryParse("abc", parsed));
    CHECK(!Date::tryParse("2026-09", parsed));
}

// ---------- 3. 计划的存盘格式 ----------
void testTaskLine() {
    beginCase("一条计划 <-> 一行文本");
    Task original(7, Date(2026, 9, 15), "写高数作业", true);
    const std::string line = original.toLine();

    Task restored;
    CHECK(Task::fromLine(line, restored));
    CHECK_EQ(restored.id(), 7);
    CHECK_EQ(restored.date().toString(), std::string("2026-09-15"));
    CHECK_EQ(restored.title(), std::string("写高数作业"));
    CHECK_EQ(restored.done(), true);

    beginCase("内容里混进 Tab 也不会弄坏文件格式");
    Task tricky(8, Date(2026, 9, 15), std::string("买\t牛奶"), false);
    Task back;
    CHECK(Task::fromLine(tricky.toLine(), back));
    CHECK_EQ(back.title(), std::string("买 牛奶"));

    beginCase("格式不对的行要被拒绝");
    Task dummy;
    CHECK(!Task::fromLine("这不是一条合法数据", dummy));
    CHECK(!Task::fromLine("abc\t2026-09-15\t0\tx", dummy));
}

// ---------- 4. 读写文件 ----------
void testRepository() {
    beginCase("存盘再读回来");

    const std::filesystem::path dir =
        std::filesystem::temp_directory_path() / "schedule_test_data";
    std::filesystem::remove_all(dir);
    const std::string path = (dir / "tasks.txt").string();

    {
        TextFileRepository repo(path);
        std::vector<Task>  tasks;
        tasks.emplace_back(1, Date(2026, 9, 15), "买牛奶", false);
        tasks.emplace_back(2, Date(2026, 9, 15), "写高数作业", true);
        tasks.emplace_back(3, Date(2026, 9, 16), "复习英语", false);

        std::string error;
        CHECK(repo.save(tasks, error));
        CHECK(error.empty());
    }

    {
        TextFileRepository repo(path);
        const std::vector<Task> tasks = repo.load();
        CHECK_EQ(tasks.size(), std::size_t(3));
        CHECK_EQ(repo.skippedLines(), 0);
        CHECK_EQ(tasks[0].title(), std::string("买牛奶"));
        CHECK_EQ(tasks[1].done(), true);
        CHECK_EQ(tasks[2].date().toString(), std::string("2026-09-16"));
    }

    beginCase("文件不存在时读出来是空的（不该崩）");
    {
        TextFileRepository repo((dir / "not_exist.txt").string());
        CHECK_EQ(repo.load().size(), std::size_t(0));
    }

    std::filesystem::remove_all(dir);
}

// ---------- 5. 业务规则 ----------
void testService() {
    beginCase("加、查、打勾、删");

    const std::filesystem::path dir =
        std::filesystem::temp_directory_path() / "schedule_test_service";
    std::filesystem::remove_all(dir);
    const std::string path = (dir / "tasks.txt").string();

    TextFileRepository repo(path);
    ScheduleService    service(repo);
    std::string        error;
    CHECK(service.load(error));

    const Date day(2026, 9, 15);
    const Date other(2026, 9, 16);

    const int id1 = service.addTask(day, "买牛奶");
    const int id2 = service.addTask(day, "写高数作业");
    const int id3 = service.addTask(other, "复习英语");
    CHECK_EQ(id1, 1);
    CHECK_EQ(id2, 2);
    CHECK_EQ(id3, 3);

    CHECK_EQ(service.addTask(day, "   "), -1);      // 空白内容不加

    CHECK_EQ(service.countOn(day), 2);              // 分日期筛选
    CHECK_EQ(service.countOn(other), 1);
    CHECK_EQ(service.countDoneOn(day), 0);

    CHECK(service.toggleTask(id2));
    CHECK_EQ(service.countDoneOn(day), 1);

    const Task* task = service.findById(id2);
    CHECK(task != nullptr);
    CHECK(task != nullptr && task->done());

    CHECK(service.toggleTask(id2));                  // 再打一次 = 取消
    CHECK_EQ(service.countDoneOn(day), 0);

    CHECK(!service.toggleTask(999));                 // 不存在的编号要安全返回 false
    CHECK(!service.removeTask(999));

    CHECK(service.save(error));
    CHECK(error.empty());

    beginCase("重新打开程序后，编号接着往下走");
    {
        TextFileRepository repo2(path);
        ScheduleService    service2(repo2);
        CHECK(service2.load(error));
        CHECK_EQ(service2.size(), std::size_t(3));
        CHECK_EQ(service2.countOn(day), 2);
        const int id4 = service2.addTask(day, "新的一条");
        CHECK_EQ(id4, 4);                            // 不能从 1 重新开始
        CHECK_EQ(service2.countOn(day), 3);
        CHECK(service2.save(error));                 // 落盘，供下一组用例读取
    }

    beginCase("删除");
    {
        TextFileRepository repo3(path);
        ScheduleService    service3(repo3);
        CHECK(service3.load(error));
        CHECK_EQ(service3.size(), std::size_t(4));
        CHECK(service3.removeTask(1));
        CHECK_EQ(service3.size(), std::size_t(3));
        CHECK(service3.findById(1) == nullptr);
    }

    std::filesystem::remove_all(dir);
}

// ---------- 6. 字符串工具 ----------
void testStrUtil() {
    beginCase("去空格 / 切分 / 转整数");
    CHECK_EQ(strutil::trim("  你好  "), std::string("你好"));
    CHECK_EQ(strutil::trim("\t\r\n "), std::string(""));
    CHECK_EQ(strutil::split("a\tb\tc", '\t').size(), std::size_t(3));

    int value = 0;
    CHECK(strutil::tryParseInt("42", value));
    CHECK_EQ(value, 42);
    CHECK(strutil::tryParseInt(" -7 ", value));
    CHECK_EQ(value, -7);
    CHECK(!strutil::tryParseInt("4a", value));
    CHECK(!strutil::tryParseInt("", value));

    CHECK_EQ(strutil::displayWidth("abc"), std::size_t(3));
    CHECK_EQ(strutil::displayWidth("中文"), std::size_t(4));
}

}  // namespace

int main() {
    std::printf("========== 开始测试 ==========\n");

    testDateBasics();
    testDateMath();
    testTaskLine();
    testRepository();
    testService();
    testStrUtil();

    std::printf("========== 测试结束 ==========\n");
    std::printf("用例 %d 组，断言 %d 条，失败 %d 条\n", g_cases, g_total, g_failed);

    if (g_failed == 0) {
        std::printf("全部通过。\n");
        return 0;
    }
    std::printf("有失败，请检查上面的输出。\n");
    return 1;
}
