# 用 CMake 编译（可选，如果你的电脑装了 CMake 或者用 CLion / VS）
#
#   cmake -S . -B build -G "MinGW Makefiles"
#   cmake --build build
#
# 没有 CMake 也没关系：直接双击 build.bat 也能编译。

cmake_minimum_required(VERSION 3.16)
project(ScheduleApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------- 主程序 ----------
file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_executable(ScheduleApp ${APP_SOURCES})
target_include_directories(ScheduleApp PRIVATE "${CMAKE_SOURCE_DIR}/include")

if (MSVC)
    # 让源码里的中文按 UTF-8 编译，否则 MSVC 会当成 GBK
    target_compile_options(ScheduleApp PRIVATE /utf-8)
endif()

# ---------- 测试程序（复用业务代码，但不含 main.cpp）----------
set(TEST_SOURCES
    "${CMAKE_SOURCE_DIR}/tests/test_main.cpp"
    "${CMAKE_SOURCE_DIR}/src/model/Date.cpp"
    "${CMAKE_SOURCE_DIR}/src/model/Task.cpp"
    "${CMAKE_SOURCE_DIR}/src/storage/TextFileRepository.cpp"
    "${CMAKE_SOURCE_DIR}/src/service/ScheduleService.cpp"
    "${CMAKE_SOURCE_DIR}/src/util/StrUtil.cpp"
)

add_executable(ScheduleTests ${TEST_SOURCES})
target_include_directories(ScheduleTests PRIVATE "${CMAKE_SOURCE_DIR}/include")

if (MSVC)
    target_compile_options(ScheduleTests PRIVATE /utf-8)
endif()
