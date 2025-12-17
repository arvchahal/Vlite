#pragma once

#include <string>

namespace vlite {

    struct VideoMetadata {
        int width;
        int height;
        double fps;
        int total_frames;
        double duration;        // in seconds
        std::string codec_name;
        int bitrate;
    };

}
