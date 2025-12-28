cmake_minimum_required(VERSION 3.28)
project(Vlite VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find FFmpeg libraries
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBAV REQUIRED IMPORTED_TARGET
    libavdevice libavfilter libavformat libavcodec
    libswresample libswscale libavutil
)

# Collect source files by component
file(GLOB CORE_SOURCES "src/vlite-core/*.cpp")
file(GLOB SAMPLING_SOURCES "src/vlite-sampling/*.cpp")

# Exclude video_loader.cpp (it has main) from library
list(REMOVE_ITEM CORE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/vlite-core/video_loader.cpp")

# Build the Vlite library
add_library(vlite STATIC
    ${CORE_SOURCES}
    ${SAMPLING_SOURCES}
)

# Build test executable
add_executable(vlite-test src/vlite-core/video_loader.cpp)
target_link_libraries(vlite-test vlite)

# Include directories for the library
target_include_directories(vlite
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Link FFmpeg to the library
target_link_libraries(vlite PUBLIC PkgConfig::LIBAV)