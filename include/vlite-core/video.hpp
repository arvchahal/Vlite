#pragma once

#include "frame.h"
#include "types.hpp"
#include <vector>
#include <string>

namespace vlite {

    class Video {
    public:
        Video() = default;

        // Load video from file
        bool load(const std::string& file_path);

        // Access frames
        const std::vector<Frame>& get_frames() const { return frames_; }
        std::vector<Frame>& get_frames() { return frames_; }

        // Access metadata
        const VideoMetadata& get_metadata() const { return metadata_; }

        // Convenience methods (delegate to utility functions)
        void resize_all(int width, int height);
        Video sample_uniform(int num_frames) const;
        Video sample_random(int num_frames) const;

        // Frame access
        const Frame& operator[](size_t index) const { return frames_[index]; }
        Frame& operator[](size_t index) { return frames_[index]; }
        size_t size() const { return frames_.size(); }
        bool empty() const { return frames_.empty(); }

    private:
        std::vector<Frame> frames_;
        VideoMetadata metadata_;
    };

}
