cmake_minimum_required (VERSION 2.8) #最低要求的CMake版本
project(vote) # 项目名称

set(CMAKE_BUILD_TYPE "debug")

link_directories(/usr/lib/hiredis) #添加动态连接库的路径


file(GLOB SRC_FILE *.cpp) # 建立变量SRC_FILE为目录下.cpp文件列表
add_executable (${PROJECT_NAME} ${SRC_FILE}) # 要求编译可执行文件


target_link_libraries(${PROJECT_NAME} -lhiredis)
