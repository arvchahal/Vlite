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

add_executable(Vlite
        src/vlite-core/video_loader.cpp
        include/vlite-core/frame.h
        include/vlite-core/video.h
        include/vlite-core/pipeline.h
        src/vlite-core/pipeline.cpp
        src/vlite-core/video.cpp
        include/vlite-sampling/sampler.h
        include/vlite-sampling/uniform.h
        include/vlite-sampling/random.h
        include/vlite-sampling/temporal.h
        include/vlite-ml/tensor.h
        src/vlite-sampling/uniform.cpp)
target_link_libraries(Vlite
        PkgConfig::LIBAV
)