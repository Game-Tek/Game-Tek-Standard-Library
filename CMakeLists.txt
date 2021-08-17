cmake_minimum_required(VERSION 3.20)

project(GTSL VERSION 1.0.0)

# requirements
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(GTSL STATIC
"${PROJECT_SOURCE_DIR}/src/Memory.cpp"
"${PROJECT_SOURCE_DIR}/src/System.cpp"
"${PROJECT_SOURCE_DIR}/src/Math/Math.cpp"
"${PROJECT_SOURCE_DIR}/src/Math/AxisAngle.cpp"
"${PROJECT_SOURCE_DIR}/src/Math/Matrix4.cpp"
"${PROJECT_SOURCE_DIR}/src/Math/Plane.cpp"
"${PROJECT_SOURCE_DIR}/src/Math/Quaternion.cpp"
"${PROJECT_SOURCE_DIR}/src/Math/Rotator.cpp"
"${PROJECT_SOURCE_DIR}/src/Math/Vectors.cpp")

target_include_directories(GTSL PUBLIC "${PROJECT_SOURCE_DIR}")
target_compile_options(GTSL PRIVATE $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-Wall>$<$<CXX_COMPILER_ID:MSVC>:/W4>)
target_compile_options(GTSL PRIVATE $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-fno-rtti>$<$<CXX_COMPILER_ID:MSVC>:/GR->)

install(TARGETS GTSL DESTINATION lib)