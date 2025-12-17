#pragma once

#include "frame.h"
#include <vector>

namespace vlite {

    // Sample N frames uniformly spaced across the video
    // Example: uniform_sample(frames, 8) from 100 frames -> indices: 0, 14, 28, 42, 57, 71, 85, 99
    std::vector<Frame> uniform_sample(const std::vector<Frame>& frames, int num_samples);

    // Sample N frames randomly (with or without replacement)
    std::vector<Frame> random_sample(const std::vector<Frame>& frames, int num_samples, bool with_replacement = false);

    // Get frame at specific index
    Frame frame_at_index(const std::vector<Frame>& frames, int index);

    // Get frame closest to timestamp (in seconds)
    Frame frame_at_timestamp(const std::vector<Frame>& frames, double timestamp);

    // Get frame indices for uniform sampling (useful for planning)
    std::vector<int> get_uniform_indices(int total_frames, int num_samples);

}
