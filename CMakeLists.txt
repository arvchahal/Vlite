cmake_minimum_required(VERSION 3.28)
project(Vlite VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find LibTorch
# Try to find torch from the build environment
if(NOT DEFINED Torch_DIR)
    # First, try to find Python if not already set
    if(NOT DEFINED PYTHON_EXECUTABLE)
        find_package(Python3 COMPONENTS Interpreter REQUIRED)
        set(PYTHON_EXECUTABLE ${Python3_EXECUTABLE})
    endif()

    # Use Python to find torch location
    execute_process(
        COMMAND ${PYTHON_EXECUTABLE} -c "import torch; print(torch.utils.cmake_prefix_path)"
        OUTPUT_VARIABLE TORCH_CMAKE_PATH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
        RESULT_VARIABLE TORCH_FIND_RESULT
    )

    if(TORCH_FIND_RESULT EQUAL 0 AND TORCH_CMAKE_PATH)
        message(STATUS "Found Torch at: ${TORCH_CMAKE_PATH}")
        set(CMAKE_PREFIX_PATH "${TORCH_CMAKE_PATH}")
    else()
        message(FATAL_ERROR "Could not find PyTorch. Please install: pip install torch")
    endif()
endif()

find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

# Find FFmpeg libraries
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBAV REQUIRED IMPORTED_TARGET
        libavdevice libavfilter libavformat libavcodec
        libswresample libswscale libavutil
)

# Collect source files by component
file(GLOB CORE_SOURCES "src/vlite-core/*.cpp")
file(GLOB SAMPLING_SOURCES "src/vlite-sampling/*.cpp")
file(GLOB TENSOR_SOURCES "src/vlite-tensor/*.cpp")

# Exclude video_loader.cpp (it has main) from library
list(REMOVE_ITEM CORE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/vlite-core/video_loader.cpp")

# Build the Vlite library
add_library(vlite STATIC
        ${TENSOR_SOURCES}
        ${CORE_SOURCES}
        ${SAMPLING_SOURCES}
        include/vlite-tensor/tensor.h
        src/vlite-tensor/tensor.cpp
)

# Build test executable
add_executable(vlite-test src/vlite-core/video_loader.cpp)
target_link_libraries(vlite-test vlite)

# Include directories for the library
target_include_directories(vlite
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        ${TORCH_INCLUDE_DIRS}
)

# Link FFmpeg and LibTorch to the library
target_link_libraries(vlite PUBLIC
        PkgConfig::LIBAV
        ${TORCH_LIBRARIES}
)

# Python bindings with pybind11
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# Try to find pybind11 in various locations
if(EXISTS "$ENV{HOME}/libs/pybind11/CMakeLists.txt")
    add_subdirectory($ENV{HOME}/libs/pybind11 ${CMAKE_BINARY_DIR}/pybind11)
else()
    # Try to find it via Python
    execute_process(
        COMMAND ${PYTHON_EXECUTABLE} -c "import pybind11; print(pybind11.get_cmake_dir())"
        OUTPUT_VARIABLE PYBIND11_CMAKE_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    if(PYBIND11_CMAKE_DIR)
        find_package(pybind11 CONFIG REQUIRED PATHS ${PYBIND11_CMAKE_DIR})
    else()
        message(FATAL_ERROR "pybind11 not found. Install with: pip install pybind11")
    endif()
endif()

pybind11_add_module(vlite_python
    vlite-pybind/main.cpp
    vlite-pybind/vlite-core.cpp
    vlite-pybind/vlite-sampling.cpp
    vlite-pybind/vlite-tensor.cpp
)

target_link_libraries(vlite_python PRIVATE vlite)
set_target_properties(vlite_python PROPERTIES OUTPUT_NAME vlite)

# ============================================================================
# Google Test Setup
# ============================================================================
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Collect all test files
file(GLOB_RECURSE TEST_SOURCES "tests/*-tests/*.cpp")

# Build test executable
add_executable(vlite_tests ${TEST_SOURCES})

target_link_libraries(vlite_tests
    gtest
    gtest_main
    vlite
)

# Auto-discover tests
include(GoogleTest)
gtest_discover_tests(vlite_tests)