cmake_minimum_required(VERSION 3.10)
project(EldenRingUtils)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src-cpp/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src-cpp/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src-cpp/*.hpp")

add_library(eldenRingUtils STATIC ${SOURCES})

target_include_directories(eldenRingUtils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src-cpp)

set(LIB_DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/lib-cpp/bin)
set(INCLUDE_DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/lib-cpp/include)

file(MAKE_DIRECTORY ${LIB_DESTINATION})
file(MAKE_DIRECTORY ${INCLUDE_DESTINATION})

add_custom_command(TARGET eldenRingUtils POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:eldenRingUtils> ${LIB_DESTINATION}/$<TARGET_FILE_NAME:eldenRingUtils>
)

add_custom_command(TARGET eldenRingUtils POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_CURRENT_SOURCE_DIR}/src-cpp/main.h ${INCLUDE_DESTINATION}/eldenRingUtils.h
)

add_custom_target(copy_headers ALL
    DEPENDS eldenRingUtils
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_CURRENT_SOURCE_DIR}/src-cpp/main.h ${INCLUDE_DESTINATION}/eldenRingUtils.h
)

add_custom_target(copy_libs ALL
    DEPENDS eldenRingUtils
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:eldenRingUtils> ${LIB_DESTINATION}/$<TARGET_FILE_NAME:eldenRingUtils>
)