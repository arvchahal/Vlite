cmake_minimum_required(VERSION 3.28)
project(Vlite VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find LibTorch
set(CMAKE_PREFIX_PATH "$ENV{HOME}/libs/libtorch")
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
        tests/vlite-core-tests/pipeline-tests.cpp
        tests/vlite-core-tests/video-tests.cpp
        tests/vlite-core-tests/frame-tests.cpp
        tests/vlite-sampling-tests/temporal-tests.cpp
        tests/vlite-sampling-tests/uniform-tests.cpp
        tests/vlite-sampling-tests/random-tests.cpp
        tests/vlite-sampling-tests/random-segment-tests.cpp
        tests/vlite-sampling-tests/sampler-tests.cpp
        tests/vlite-tensor-tests/tensor-tests.cpp
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
add_subdirectory($ENV{HOME}/libs/pybind11)

pybind11_add_module(vlite_python
    vlite-pybind/main.cpp
    vlite-pybind/vlite-core.cpp
    vlite-pybind/vlite-sampling.cpp
    vlite-pybind/vlite-tensor.cpp
)

target_link_libraries(vlite_python PRIVATE vlite)
set_target_properties(vlite_python PROPERTIES OUTPUT_NAME vlite)

# Add tests subdirectory
add_subdirectory(tests)