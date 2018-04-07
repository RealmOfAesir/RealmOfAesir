cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)
project(RealmOfAesirWorldSub)

# for lua
SET(CMAKE_EXE_LINKER_FLAGS  "-rdynamic")

add_subdirectory(src)

enable_testing()
add_subdirectory(test)
add_subdirectory(benchmarks)
