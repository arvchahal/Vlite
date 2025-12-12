cmake_minimum_required(VERSION 3.28)
project(Vlite)

set(CMAKE_CXX_STANDARD 20)

# Find FFmpeg libraries
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBAV REQUIRED IMPORTED_TARGET
        libavdevice
        libavfilter
        libavformat
        libavcodec
        libswresample
        libswscale
        libavutil
)
add_executable(Vlite main.cpp)
target_link_libraries(Vlite
        PkgConfig::LIBAV
)