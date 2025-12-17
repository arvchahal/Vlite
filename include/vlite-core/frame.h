#pragma once

#include <vector>
#include <ostream>
#include <cstdint>

namespace vlite {

    enum class PixelFormat {
        RGB24,
        GRAY8
    };

    struct Frame {
        std::vector<uint8_t> frameData;
        int width;
        int height;
        PixelFormat format = PixelFormat::RGB24;
        int frame_index = -1;      // Position in video sequence
        double timestamp = 0.0;     // Time in seconds
    };

    // Stream operator for debugging
    inline std::ostream& operator<<(std::ostream& os, const Frame& frame) {
        os << "Frame #" << frame.frame_index
           << " @ " << frame.timestamp << "s: "
           << frame.width << "x" << frame.height
           << " (" << (frame.format == PixelFormat::RGB24 ? "RGB24" : "GRAY8") << ")";
        return os;
    }

}

#endif
