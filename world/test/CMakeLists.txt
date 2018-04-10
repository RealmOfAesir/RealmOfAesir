cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)
project(RealmOfAesirWorldTest CXX)

file(GLOB_RECURSE PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/../src/*.cpp)
list(REMOVE_ITEM PROJECT_SOURCES "${PROJECT_SOURCE_DIR}/../src/main.cpp")

file(GLOB PROJECT_TEST_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp ${PROJECT_SOURCE_DIR}/test_helpers/*.cpp)
file(GLOB EASYLOGGING_SOURCE ${EXTERNAL_DIR}/easyloggingpp/src/easylogging++.cc)
file(GLOB_RECURSE COMMON_SOURCE ${TOP_DIR}/common/src/*.cpp)
add_executable(RealmOfAesirWorldTest ${PROJECT_SOURCES} ${PROJECT_TEST_SOURCES} ${COMMON_SOURCE} ${EASYLOGGING_SOURCE} main.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DELPP_NO_LOG_TO_FILE ")

add_test(NAME MyTest COMMAND RealmOfAesirWorldTest)

include_directories("${TOP_DIR}/common/src")
include_directories("${EXTERNAL_DIR}/librdkafka/src-cpp")
include_directories("${EXTERNAL_DIR}/librdkafka/src")
include_directories("${EXTERNAL_DIR}/easyloggingpp/src")
include_directories("${EXTERNAL_DIR}/cereal/include")
include_directories("${EXTERNAL_DIR}/di/include")
include_directories("${EXTERNAL_DIR}/libpqxx/include")
include_directories("${EXTERNAL_DIR}/json/include")
include_directories("${EXTERNAL_DIR}/entt/src")
include_directories("${EXTERNAL_DIR}/readerwriterqueue")
include_directories("${EXTERNAL_DIR}/lz4/lib")
include_directories("${EXTERNAL_DIR}/pcg-cpp/include")
include_directories("${EXTERNAL_DIR}/Catch2/single_include")
include_directories("${PROJECT_SOURCE_DIR}/../src/")

find_library(RDKAFKA_LIBRARY rdkafka HINTS ${EXTERNAL_DIR}/librdkafka/src)
find_library(RDKAFKAPP_LIBRARY rdkafka++ HINTS ${EXTERNAL_DIR}/librdkafka/src-cpp)
find_library(PQXX_LIBRARY pqxx HINTS ${EXTERNAL_DIR}/libpqxx/src/.libs)
find_library(LZ_LIBRARY lz4 HINTS ${EXTERNAL_DIR}/lz4/lib)

if(NOT RDKAFKA_LIBRARY)
    message(FATAL_ERROR "rdkafka not found")
endif()

if(NOT RDKAFKAPP_LIBRARY)
    message(FATAL_ERROR "rdkafkapp not found")
endif()

if(NOT PQXX_LIBRARY)
    message(FATAL_ERROR "pqxx not found")
endif()

if(NOT LZ_LIBRARY)
    message(FATAL_ERROR "LZ_LIBRARY not found")
endif()

target_link_libraries(RealmOfAesirWorldTest PUBLIC ${RDKAFKAPP_LIBRARY})
target_link_libraries(RealmOfAesirWorldTest PUBLIC ${RDKAFKA_LIBRARY})
target_link_libraries(RealmOfAesirWorldTest PUBLIC ${PQXX_LIBRARY})
target_link_libraries(RealmOfAesirWorldTest PUBLIC ${LZ_LIBRARY})

find_package(PostgreSQL REQUIRED)
include_directories(${PostgreSQL_INCLUDE_DIRS})
target_link_libraries(RealmOfAesirWorldTest PUBLIC ${PostgreSQL_LIBRARIES})

find_package( ZLIB REQUIRED )
include_directories( ${ZLIB_INCLUDE_DIRS} )
target_link_libraries(RealmOfAesirWorldTest PUBLIC ${ZLIB_LIBRARIES} )

find_package (Threads)
target_link_libraries (RealmOfAesirWorldTest PUBLIC ${CMAKE_THREAD_LIBS_INIT})

find_package(OpenSSL REQUIRED)
target_link_libraries (RealmOfAesirWorldTest PUBLIC ${OPENSSL_LIBRARIES})
