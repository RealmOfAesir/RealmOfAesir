cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR)
project(Test CXX)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DELPP_NO_LOG_TO_FILE ")

file(GLOB PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp ${PROJECT_SOURCE_DIR}/test_helpers/*.cpp)
file(GLOB EASYLOGGING_SOURCE ${CMAKE_SOURCE_DIR}/external/easyloggingpp/src/easylogging++.cc)
add_executable(Test ${PROJECT_SOURCES} ${EASYLOGGING_SOURCE} main.cpp)

add_test(NAME MyTest COMMAND Test)


include_directories("${CMAKE_SOURCE_DIR}/src")
include_directories("${CMAKE_SOURCE_DIR}/external/cereal/include")
include_directories("${CMAKE_SOURCE_DIR}/external/easyloggingpp/src")
include_directories("${CMAKE_SOURCE_DIR}/external/json/src")
include_directories("${CMAKE_SOURCE_DIR}/external/di/include")
include_directories("${CMAKE_SOURCE_DIR}/external/Catch/include")

target_link_libraries(Test PUBLIC RealmOfAesirCommon)