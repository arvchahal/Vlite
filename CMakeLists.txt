cmake_minimum_required(VERSION 3.28)
project(Vlite VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find LibTorch
set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/libtorch")
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