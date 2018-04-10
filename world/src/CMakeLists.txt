cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)
project(RealmOfAesirWorldSrc CXX)

include_directories("${PROJECT_SOURCE_DIR}")

file(GLOB_RECURSE PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB EASYLOGGING_SOURCE ${EXTERNAL_DIR}/easyloggingpp/src/easylogging++.cc)
file(GLOB_RECURSE COMMON_SOURCE ${TOP_DIR}/common/src/*.cpp)
add_executable(RealmOfAesirWorldSrc ${EASYLOGGING_SOURCE} ${COMMON_SOURCE} ${PROJECT_SOURCES} )

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
include_directories("${EXTERNAL_DIR}/LuaJIT/src")
include_directories("${EXTERNAL_DIR}/lz4/lib")
include_directories("${EXTERNAL_DIR}/pcg-cpp/include")

find_library(RDKAFKA_LIBRARY rdkafka HINTS ${EXTERNAL_DIR}/librdkafka/src)
find_library(RDKAFKAPP_LIBRARY rdkafka++ HINTS ${EXTERNAL_DIR}/librdkafka/src-cpp)
find_library(PQXX_LIBRARY pqxx HINTS ${EXTERNAL_DIR}/libpqxx/src/.libs)
find_library(LUAJIT_LIBRARY luajit HINTS ${EXTERNAL_DIR}/LuaJIT/src)
find_library(LZ_LIBRARY lz4 HINTS ${EXTERNAL_DIR}/lz4/lib)

if(NOT RDKAFKA_LIBRARY)
    message(FATAL_ERROR "rdkafka not found")
endif()

if(NOT RDKAFKAPP_LIBRARY)
    message(FATAL_ERROR "rdkafkapp not found")
endif()

if(NOT PQXX_LIBRARY)
    message(FATAL_ERROR "libpqxx not found")
endif()

if(NOT LUAJIT_LIBRARY)
    message(FATAL_ERROR "LUAJIT_LIBRARY not found")
endif()

if(NOT LZ_LIBRARY)
    message(FATAL_ERROR "LZ_LIBRARY not found")
endif()

target_link_libraries(RealmOfAesirWorldSrc PUBLIC ${RDKAFKAPP_LIBRARY})
target_link_libraries(RealmOfAesirWorldSrc PUBLIC ${RDKAFKA_LIBRARY})
target_link_libraries(RealmOfAesirWorldSrc PUBLIC ${PQXX_LIBRARY})
target_link_libraries(RealmOfAesirWorldSrc PUBLIC ${LUAJIT_LIBRARY})
target_link_libraries(RealmOfAesirWorldSrc PUBLIC ${LZ_LIBRARY})

find_package(PostgreSQL REQUIRED)
include_directories(${PostgreSQL_INCLUDE_DIRS})
target_link_libraries(RealmOfAesirWorldSrc PUBLIC ${PostgreSQL_LIBRARIES})

find_package( ZLIB REQUIRED )
include_directories( ${ZLIB_INCLUDE_DIRS} )
target_link_libraries(RealmOfAesirWorldSrc PUBLIC ${ZLIB_LIBRARIES} )

find_package (Threads)
target_link_libraries (RealmOfAesirWorldSrc PUBLIC ${CMAKE_THREAD_LIBS_INIT})

find_package(OpenSSL REQUIRED)
target_link_libraries (RealmOfAesirWorldSrc PUBLIC ${OPENSSL_LIBRARIES})
