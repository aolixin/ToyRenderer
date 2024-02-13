cmake_minimum_required(VERSION 3.22)
project(testProj4)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SRCS src/test1.cpp)
add_executable(${PROJECT_NAME} ${SRCS})


target_include_directories(${PROJECT_NAME} PRIVATE include)