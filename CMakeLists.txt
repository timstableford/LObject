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

set(SRCS src/LObject.cpp)

# Build the shared library
add_library(LObject_shared SHARED ${SRCS})
set_target_properties(LObject_shared PROPERTIES OUTPUT_NAME "lobject")
target_link_libraries(LObject_shared DynamicBuffer)
target_include_directories(LObject_shared PUBLIC src)

# Build the static library
add_library(LObject_static STATIC ${SRCS})
set_target_properties(LObject_static PROPERTIES OUTPUT_NAME "lobject")
target_link_libraries(LObject_static DynamicBuffer)
target_include_directories(LObject_static PUBLIC src)

if (BUILD_TESTS)
if (NOT gtest_SOURCE_DIR)
add_subdirectory(lib/googletest)
endif(NOT gtest_SOURCE_DIR)

enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR} .)

add_executable(${PROJECT_NAME}_Test Test.cpp)
target_link_libraries(${PROJECT_NAME}_Test gtest gtest_main LObject_static DynamicBuffer)
endif()
