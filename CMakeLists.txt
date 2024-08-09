cmake_minimum_required(VERSION 3.21)
project(tools)

set(CMAKE_CXX_STANDARD 20)

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS "/permissive-")
endif ()

include_directories(include)

find_package(nlohmann_json CONFIG REQUIRED)
find_package(nanomsg CONFIG REQUIRED)

add_subdirectory(src)
add_subdirectory(tests)