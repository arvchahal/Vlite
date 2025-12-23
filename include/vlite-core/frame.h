#pragma once

#include <cstdint>
#include <ostream>
#include <vector>

namespace vlite {

enum class PixelFormat : unsigned char { RGB24, GRAY8 };

struct Frame {
    std::vector<uint8_t> frameData;
    double timestamp = 0.0; // Time in seconds
    int width;
    int height;
    int frame_index = -1;                    // Position in video sequence
    PixelFormat format = PixelFormat::RGB24; //
};

inline std::ostream &operator<<(std::ostream &os, const Frame &frame) {
    os << "Frame #" << frame.frame_index << " @ " << frame.timestamp
       << "s: " << frame.width << "x" << frame.height << " ("
       << (frame.format == PixelFormat::RGB24 ? "RGB24" : "GRAY8") << ")";
    return os;
}

} // namespace vlite
