#pragma once

#include "frame.h"
#include <vector>

namespace vlite {

    // Resize a single frame to new dimensions
    // Uses bilinear interpolation via libswscale
    Frame resize_frame(const Frame& frame, int new_width, int new_height);

    // Resize all frames in a vector
    std::vector<Frame> resize_frames(const std::vector<Frame>& frames, int new_width, int new_height);

    // Rescale pixel values from [0, 255] to [0.0, 1.0]
    // Converts uint8_t data to float32
    std::vector<float> normalize_frame(const Frame& frame);

    // Convert frame data to float32 without normalization
    std::vector<float> to_float32(const Frame& frame);

}
