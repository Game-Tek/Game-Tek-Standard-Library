cmake_minimum_required(VERSION 3.25)

project(GTSL VERSION 1.0.0 LANGUAGES CXX)

set(BUILD_TESTING OFF CACHE BOOL "Builds any Tests")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE PROJECT_FILES "GTSL/*.*")
add_library(GTSL ${PROJECT_FILES})
target_include_directories(GTSL PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/GTSL>
        $<INSTALL_INTERFACE:include/GTSL>)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
add_subdirectory("vendor/glfw")
target_link_libraries(GTSL PUBLIC glfw)

install(TARGETS GTSL DESTINATION lib)


if(${BUILD_TESTING})
    add_executable(Test "Test/main.cpp")
    target_include_directories(Test PUBLIC ${GTSL_SOURCE_DIR})
    target_link_libraries(Test GTSL)
endif()