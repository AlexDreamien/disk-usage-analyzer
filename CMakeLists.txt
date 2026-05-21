cmake_minimum_required(VERSION 3.20)
project(dux LANGUAGES CXX VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

if(MSVC)
    add_compile_options(/W4 /permissive- /Zc:__cplusplus)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_library(dux_core
    src/format.cpp
    src/scan.cpp
    src/report.cpp
)
target_include_directories(dux_core PUBLIC include)

add_executable(dux src/main.cpp)
target_link_libraries(dux PRIVATE dux_core)

option(DUX_BUILD_TESTS "Build unit tests" ON)
if(DUX_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
