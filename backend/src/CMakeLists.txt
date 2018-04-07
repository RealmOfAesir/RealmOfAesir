cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)
project(RealmOfAesirBackendSrc CXX)

include_directories("${PROJECT_SOURCE_DIR}")

file(GLOB_RECURSE PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB EASYLOGGING_SOURCE ${EXTERNAL_DIR}/easyloggingpp/src/easylogging++.cc)
file(GLOB_RECURSE COMMON_SOURCE ${TOP_DIR}/common/src/*.cpp)
add_executable(RealmOfAesirBackendSrc ${EASYLOGGING_SOURCE} ${COMMON_SOURCE} ${PROJECT_SOURCES} )

include_directories("${TOP_DIR}/common/src")
include_directories("${EXTERNAL_DIR}/librdkafka/src-cpp")
include_directories("${EXTERNAL_DIR}/librdkafka/src")
include_directories("${EXTERNAL_DIR}/easyloggingpp/src")
include_directories("${EXTERNAL_DIR}/cereal/include")
include_directories("${EXTERNAL_DIR}/di/include")
include_directories("${EXTERNAL_DIR}/libpqxx/include")
include_directories("${EXTERNAL_DIR}/json/include")
include_directories("${EXTERNAL_DIR}/libsodium/src/libsodium/include")

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

target_link_libraries(RealmOfAesirBackendSrc PUBLIC ${RDKAFKA_LIBRARY})
target_link_libraries(RealmOfAesirBackendSrc PUBLIC ${RDKAFKAPP_LIBRARY})
target_link_libraries(RealmOfAesirBackendSrc PUBLIC ${PQXX_LIBRARY})
target_link_libraries(RealmOfAesirBackendSrc PUBLIC ${SODIUM_LIBRARY})

find_package(PostgreSQL REQUIRED)
include_directories(${PostgreSQL_INCLUDE_DIRS})
target_link_libraries(RealmOfAesirBackendSrc PUBLIC ${PostgreSQL_LIBRARIES})

find_package( ZLIB REQUIRED )
include_directories( ${ZLIB_INCLUDE_DIRS} )
target_link_libraries(RealmOfAesirBackendSrc PUBLIC ${ZLIB_LIBRARIES} )

find_package (Threads)
target_link_libraries (RealmOfAesirBackendSrc PUBLIC ${CMAKE_THREAD_LIBS_INIT})

find_package(OpenSSL REQUIRED)
target_link_libraries (RealmOfAesirBackendSrc PUBLIC ${OPENSSL_LIBRARIES})