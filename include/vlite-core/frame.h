#pragma once

#include <cstdint>
#include <ostream>
#include <vector>
#include <libavutil/pixfmt.h>
#include <iostream>

namespace vlite {
    struct Frame {
        std::vector<uint8_t> frameData;
        double timestamp = 0.0; // Time in seconds
        int width;
        int height;
        int frame_index = -1; // Position in video sequence
        AVPixelFormat format = AV_PIX_FMT_RGB24; //
    };

    inline int get_bytes_per_pixel(AVPixelFormat fmt) {
        switch (fmt) {
            case AV_PIX_FMT_GRAY8: return 1;
            case AV_PIX_FMT_RGB24: return 3;
            case AV_PIX_FMT_BGR24: return 3;
            case AV_PIX_FMT_RGBA: return 4;
            case AV_PIX_FMT_BGRA: return 4;
            case AV_PIX_FMT_RGB48: return 6;
            case AV_PIX_FMT_GRAY16: return 2;
            default:
                std::cerr << "Unsupported pixel format: " << fmt << "\n";
                return -1;
        }
    }

    // calculate stride (bytes per row)
    inline int get_stride(int width, AVPixelFormat fmt) {
        int bpp = get_bytes_per_pixel(fmt);
        if (bpp < 0) {
            std::cerr << "Cannot calculate stride for unsupported format\n";
            return -1;
        }
        return width * bpp;
    }

    inline bool is_format_supported(AVPixelFormat fmt) {
        return get_bytes_per_pixel(fmt) > 0;
    }

    inline std::ostream &operator<<(std::ostream &os, const Frame &frame) {
        os << "Frame #" << frame.frame_index << " @ " << frame.timestamp
                << "s: " << frame.width << "x" << frame.height;
        return os;
    }
} // namespace vlite
