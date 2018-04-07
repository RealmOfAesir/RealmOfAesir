cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)
project(RealmOfAesirBackendTest CXX)

file(GLOB_RECURSE PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/../src/*.cpp)
list(REMOVE_ITEM PROJECT_SOURCES "${PROJECT_SOURCE_DIR}/../src/main.cpp")

file(GLOB_RECURSE PROJECT_TEST_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB EASYLOGGING_SOURCE ${EXTERNAL_DIR}/easyloggingpp/src/easylogging++.cc)
file(GLOB_RECURSE COMMON_SOURCE ${TOP_DIR}/common/src/*.cpp)
add_executable(RealmOfAesirBackendTest ${PROJECT_SOURCES} ${PROJECT_TEST_SOURCES} ${COMMON_SOURCE} ${EASYLOGGING_SOURCE} main.cpp)

add_test(NAME RealmOfAesirBackendTest COMMAND RealmOfAesirBackendTest)

include_directories("${TOP_DIR}/common/src")
include_directories("${EXTERNAL_DIR}/librdkafka/src-cpp")
include_directories("${EXTERNAL_DIR}/librdkafka/src")
include_directories("${EXTERNAL_DIR}/easyloggingpp/src")
include_directories("${EXTERNAL_DIR}/cereal/include")
include_directories("${EXTERNAL_DIR}/di/include")
include_directories("${EXTERNAL_DIR}/libpqxx/include")
include_directories("${EXTERNAL_DIR}/json/include")
include_directories("${EXTERNAL_DIR}/libsodium/src/libsodium/include")
include_directories("${EXTERNAL_DIR}/Catch2/single_include")
include_directories("${PROJECT_SOURCE_DIR}/../src/")

find_library(RDKAFKA_LIBRARY rdkafka HINTS ${EXTERNAL_DIR}/librdkafka/src)
find_library(RDKAFKAPP_LIBRARY rdkafka++ HINTS ${EXTERNAL_DIR}/librdkafka/src-cpp)
find_library(PQXX_LIBRARY pqxx HINTS ${EXTERNAL_DIR}/libpqxx/src/.libs)
find_library(SODIUM_LIBRARY sodium HINTS ${EXTERNAL_DIR}/libsodium/src/libsodium/.libs)

if(NOT RDKAFKA_LIBRARY)
    message(FATAL_ERROR "librdkafka not found")
endif()

if(NOT RDKAFKAPP_LIBRARY)
    message(FATAL_ERROR "librdkafkapp not found")
endif()

if(NOT PQXX_LIBRARY)
    message(FATAL_ERROR "libpqxx not found")
endif()

if(NOT SODIUM_LIBRARY)
    message(FATAL_ERROR "libsodium not found")
endif()

target_link_libraries(RealmOfAesirBackendTest PUBLIC ${RDKAFKA_LIBRARY})
target_link_libraries(RealmOfAesirBackendTest PUBLIC ${RDKAFKAPP_LIBRARY})
target_link_libraries(RealmOfAesirBackendTest PUBLIC ${PQXX_LIBRARY})
target_link_libraries(RealmOfAesirBackendTest PUBLIC ${SODIUM_LIBRARY})

find_package(PostgreSQL REQUIRED)
include_directories(${PostgreSQL_INCLUDE_DIRS})
target_link_libraries(RealmOfAesirBackendTest PUBLIC ${PostgreSQL_LIBRARIES})

find_package( ZLIB REQUIRED )
include_directories( ${ZLIB_INCLUDE_DIRS} )
target_link_libraries(RealmOfAesirBackendTest PUBLIC ${ZLIB_LIBRARIES} )

find_package (Threads)
target_link_libraries (RealmOfAesirBackendTest PUBLIC ${CMAKE_THREAD_LIBS_INIT})

find_package(OpenSSL REQUIRED)
target_link_libraries (RealmOfAesirBackendTest PUBLIC ${OPENSSL_LIBRARIES})