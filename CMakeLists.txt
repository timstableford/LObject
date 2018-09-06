cmake_minimum_required (VERSION 2.6)
project (LObject)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic")
set(CMAKE_CXX_STANDARD 11)
option(BUILD_TESTS "Build all tests." OFF)

add_definitions(-D__LINUX_BUILD)

if (NOT TARGET DynamicBuffer)
add_subdirectory(lib/DynamicBuffer)
endif()

include_directories(src)

add_library(LObject SHARED src/LObject.cpp)
target_link_libraries(LObject DynamicBuffer)
target_include_directories(LObject PUBLIC src)

if (BUILD_TESTS)
if (NOT gtest_SOURCE_DIR)
add_subdirectory(lib/googletest)
endif(NOT gtest_SOURCE_DIR)

enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR} .)

add_executable(${PROJECT_NAME}_Test Test.cpp)
target_link_libraries(${PROJECT_NAME}_Test gtest gtest_main LObject DynamicBuffer)
endif()
