cmake_minimum_required(VERSION 3.10)

# 设置项目名称
project(HardwareSimulator)

# 添加包含目录
include_directories(${CMAKE_SOURCE_DIR}/src/hardwares)
include_directories(${CMAKE_SOURCE_DIR}/src/common)

# 查找所有源文件
file(GLOB BOOT_SOURCES
    "${CMAKE_SOURCE_DIR}/src/boot.c"
    "${CMAKE_SOURCE_DIR}/src/common/*.c"
    "${CMAKE_SOURCE_DIR}/src/hardwares/*.c"
)

# 设置可执行文件输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# 添加可执行文件
add_executable(boot ${BOOT_SOURCES})

