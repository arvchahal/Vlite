#pragma once

#include "frame.h"
#include "types.h"
#include <iostream>
#include <vector>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}
namespace vlite {

class Video {
      public:
        Video() = default;

        // Load video from file
        bool load(const char *file_path,  AVPixelFormat format);

        // Access frames
        const std::vector<std::shared_ptr<Frame>> &get_frames() const { return frames_; }
        std::vector<std::shared_ptr<Frame>> &get_frames() { return frames_; }

        const Frame &operator[](const size_t index) const { return *frames_[index]; }
        Frame &operator[](const size_t index) { return *frames_[index]; }
        size_t size() const { return frames_.size(); }
        bool empty() const { return frames_.empty(); }
        void push_frame(Frame f){frames_.push_back(std::make_shared<Frame>(std::move(f)));}

      private:
        std::vector<std::shared_ptr<Frame>> frames_;
        VideoMetadata metadata_;
};

} // namespace vlite
