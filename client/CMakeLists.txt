cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)
project(RealmOfAesirClient CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake_modules")

if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
endif()

include_directories("${PROJECT_SOURCE_DIR}")

file(GLOB_RECURSE PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB EASYLOGGING_SOURCE ${EXTERNAL_DIR}/easyloggingpp/src/easylogging++.cc)
file(GLOB_RECURSE COMMON_SOURCE ${TOP_DIR}/common/src/*.cpp)
add_executable(RealmOfAesirClient ${EASYLOGGING_SOURCE} ${COMMON_SOURCE} ${PROJECT_SOURCES})

include(FindOpenGL)
include(FindGLEW)

if(NOT OPENGL_GLU_FOUND)
  message(FATAL_ERROR "Either OpenGL or GLU not found")
endif()

if(NOT GLEW_FOUND)
  message(FATAL_ERROR "GLEW not found")
endif()

#versions don't seem to do anything, but it serves as a central point for which version are necessary.
find_package(SDL2 REQUIRED >= 2.0.4)
find_package(SDL2_image REQUIRED >= 2.0.0)
find_package(GLM REQUIRED >= 0.9.7)

find_library(RDKAFKA_LIBRARY rdkafka HINTS ${EXTERNAL_DIR}/librdkafka/src)
find_library(RDKAFKAPP_LIBRARY rdkafka++ HINTS ${EXTERNAL_DIR}/librdkafka/src-cpp)
find_library(PQXX_LIBRARY pqxx HINTS ${EXTERNAL_DIR}/libpqxx/src/.libs)

include_directories("${SDL2_INCLUDE_DIR}")
include_directories("${SDL2_IMAGE_INCLUDE_DIRS}")
include_directories("${OPENGL_INCLUDE_DIR}")
include_directories("${GLEW_INCLUDE_DIRS}")
include_directories("${GLM_INCLUDE_DIRS}")
include_directories("${TOP_DIR}/common/src")
include_directories("${EXTERNAL_DIR}/librdkafka/src-cpp")
include_directories("${EXTERNAL_DIR}/librdkafka/src")
include_directories("${EXTERNAL_DIR}/easyloggingpp/src")
include_directories("${EXTERNAL_DIR}/cereal/include")
include_directories("${EXTERNAL_DIR}/libpqxx/include")

if(NOT SDL2_LIBRARY)
    message(FATAL_ERROR "sdl2 not found")
endif()

if(NOT SDL2_IMAGE_LIBRARIES)
    message(FATAL_ERROR "sdl2image not found")
endif()

if(NOT OPENGL_LIBRARIES)
    message(FATAL_ERROR "opengl not found")
endif()

if(NOT GLEW_LIBRARIES)
    message(FATAL_ERROR "glew not found")
endif()

if(NOT RDKAFKAPP_LIBRARY)
    message(FATAL_ERROR "kafka++ not found")
endif()

if(NOT RDKAFKA_LIBRARY)
    message(FATAL_ERROR "kafka not found")
endif()

if(NOT PQXX_LIBRARY)
    message(FATAL_ERROR "libpqxx not found")
endif()

target_link_libraries(RealmOfAesirClient ${SDL2_LIBRARY})
target_link_libraries(RealmOfAesirClient ${SDL2_IMAGE_LIBRARIES})
target_link_libraries(RealmOfAesirClient ${OPENGL_LIBRARIES})
target_link_libraries(RealmOfAesirClient ${GLEW_LIBRARIES})
target_link_libraries(RealmOfAesirClient ${RDKAFKAPP_LIBRARY})
target_link_libraries(RealmOfAesirClient ${RDKAFKA_LIBRARY})
target_link_libraries(RealmOfAesirClient ${PQXX_LIBRARY})

find_package(PostgreSQL REQUIRED)
include_directories(${PostgreSQL_INCLUDE_DIRS})
target_link_libraries(RealmOfAesirClient ${PostgreSQL_LIBRARIES})

#post-build esque stuff
add_custom_command(
        TARGET RealmOfAesirClient POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/src/shaders/
                ${CMAKE_BINARY_DIR}/shaders)

add_custom_command(
        TARGET RealmOfAesirClient POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/assets/
                ${CMAKE_BINARY_DIR}/assets)
