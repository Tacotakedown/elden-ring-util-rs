cmake_minimum_required(VERSION 3.10)
project(EldenRingUtils)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src-cpp/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src-cpp/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src-cpp/*.hpp")

add_library(eldenRingUtils STATIC ${SOURCES})

target_include_directories(eldenRingUtils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src-cpp)
