cmake_minimum_required(VERSION 3.20)
include(FetchContent)

project(GTSL VERSION 1.0.0 LANGUAGES CXX)

option(GTSL_TESTS_ENABLED ON)

# requirements
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(GTSL INTERFACE)

target_include_directories(GTSL INTERFACE "${PROJECT_SOURCE_DIR}")

install(TARGETS GTSL DESTINATION lib)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	message("Compiling for WIN64")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
	find_package(X11 REQUIRED)
	target_link_libraries(GTSL INTERFACE ${X11_LIBRARIES})
	target_link_libraries(GTSL INTERFACE xcb)
	target_include_directories(GTSL INTERFACE ${X11_INCLUDE_DIR})
endif()

if(CMAKE_TESTING_ENABLED)
	message("CMAKE testing enabled.")

	enable_testing()
	FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip)
	# For Windows: Prevent overriding the parent project's compiler/linker settings
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	FetchContent_MakeAvailable(googletest)
	
	add_executable(GTSL_Test
	"${PROJECT_SOURCE_DIR}/test/TestMain.cpp"
	"${PROJECT_SOURCE_DIR}/test/TestVector.cpp"
	"${PROJECT_SOURCE_DIR}/test/TestFixedVector.cpp"
	"${PROJECT_SOURCE_DIR}/test/TestMath.cpp"
	"${PROJECT_SOURCE_DIR}/test/TestMemory.cpp"
	"${PROJECT_SOURCE_DIR}/test/TestString.cpp"
	"${PROJECT_SOURCE_DIR}/test/TestShortString.cpp"
	"${PROJECT_SOURCE_DIR}/test/TestHashMap.cpp"
	"${PROJECT_SOURCE_DIR}/test/TestUnicode.cpp"
	"${PROJECT_SOURCE_DIR}/test/TestRange.cpp"
	"${PROJECT_SOURCE_DIR}/test/TestOS.cpp"
	"${PROJECT_SOURCE_DIR}/test/TestParsers.cpp"
	"${PROJECT_SOURCE_DIR}/test/TestTree.cpp"
	"${PROJECT_SOURCE_DIR}/test/TestAlgorithm.cpp"
	"${PROJECT_SOURCE_DIR}/test/TestStringCommon.cpp"
	"${PROJECT_SOURCE_DIR}/test/TestBuffer.cpp"
	"test/TestCollision.cpp"
	"test/TestIndirectionTable.cpp"
	"test/TestRatioUnit.cpp")
	target_include_directories(GTSL_Test PUBLIC "${PROJECT_SOURCE_DIR}")
	target_link_libraries(GTSL_Test gtest_main)
	target_link_libraries(GTSL_Test GTSL)

	include(GoogleTest)
	gtest_discover_tests(GTSL_Test)
else()
	message("CMAKE testing disabled.")
endif()