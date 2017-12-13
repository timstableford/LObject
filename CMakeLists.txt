cmake_minimum_required (VERSION 2.6)
project (LObject)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic")
set(CMAKE_CXX_STANDARD 11)

include_directories(src)

add_library(LObject SHARED src/LObject.cpp)
target_include_directories(LObject PUBLIC src)
