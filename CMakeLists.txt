cmake_minimum_required(VERSION 3.16)
project(aomp)

set(CMAKE_CXX_STANDARD 20)

enable_testing()
include(GoogleTest)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(aomp chapter_1_tests.cpp)
target_link_libraries(aomp gtest gtest_main Threads::Threads)