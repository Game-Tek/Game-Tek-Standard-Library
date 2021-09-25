cmake_minimum_required(VERSION 3.20)
include(FetchContent)

project(GTSL VERSION 1.0.0)

option(GTSL_TESTS_ENABLED OFF)

# requirements
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(GTSL INTERFACE)

target_include_directories(GTSL INTERFACE "${PROJECT_SOURCE_DIR}")

install(TARGETS GTSL DESTINATION lib)

#enable_testing()
#find_package(GTest CONFIG REQUIRED)

if(GTSL_TESTS_ENABLED)
	FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip)
	# For Windows: Prevent overriding the parent project's compiler/linker settings
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	FetchContent_MakeAvailable(googletest)
	
	add_executable(GTSL_Test
	"${PROJECT_SOURCE_DIR}/test/TestMain.cpp"
	"${PROJECT_SOURCE_DIR}/test/TestVector.cpp"
	"${PROJECT_SOURCE_DIR}/test/TestMath.cpp"
	"${PROJECT_SOURCE_DIR}/test/TestString.cpp"
	"${PROJECT_SOURCE_DIR}/test/TestShortString.cpp"
	"${PROJECT_SOURCE_DIR}/test/TestHashMap.cpp"
	"${PROJECT_SOURCE_DIR}/test/TestUnicode.cpp"
	"${PROJECT_SOURCE_DIR}/test/TestRange.cpp"
	"${PROJECT_SOURCE_DIR}/test/TestAlgorithm.cpp"
	"${PROJECT_SOURCE_DIR}/test/TestStringCommon.cpp")
	target_include_directories(GTSL_Test PUBLIC "${PROJECT_SOURCE_DIR}")
	target_link_libraries(GTSL_Test gtest_main)
endif()

#include(GoogleTest)
#gtest_discover_tests(GTSL_Test)