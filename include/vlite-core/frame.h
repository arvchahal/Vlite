#pragma once

#include <cstdint>
#include <ostream>
#include <vector>
#include <libavutil/pixfmt.h>

namespace vlite {


struct Frame {
    std::vector<uint8_t> frameData;
    double timestamp = 0.0; // Time in seconds
    int width;
    int height;
    int frame_index = -1;                    // Position in video sequence
    AVPixelFormat format = AV_PIX_FMT_RGB24; //
};

inline std::ostream &operator<<(std::ostream &os, const Frame &frame) {
    os << "Frame #" << frame.frame_index << " @ " << frame.timestamp
       << "s: " << frame.width << "x" << frame.height;
    return os;
}

} // namespace vlite
