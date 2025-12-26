#pragma once

#include "frame.h"
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
    //private vars
    std::vector<std::shared_ptr<Frame>> frames_;
    std::string video_name;
      public:
        Video() = default;
        //important shit
        [[nodiscard]] bool load(const char *file_path,  AVPixelFormat format);
        [[nodiscard]] bool save(const char *output_path, AVCodecID codec_id = AV_CODEC_ID_H264, int fps = 30);
        [[nodiscard]] bool load_resize(const char* file_path, AVPixelFormat format, int newWidth, int newHeight);
        [[nodiscard]] bool resize(AVPixelFormat oldFormat, int oldWidth, int oldHeight, AVPixelFormat newFormat,
            int newWidth, int newHeight);



        //getter-esque methods
        std::vector<std::shared_ptr<Frame>> &get_frames() { return frames_; }
        // std::vector<std::shared_ptr<Frame>> &get_frames() const { return  std::vector<std::shared_ptr<Frame>>{frames_.begin(),frames_.end()};

        // std::vector<std::shared_ptr<Frame>> get_frames() { return const_cast<std::vector<std::shared_ptr<Frame>>>(frames_); }
        std::string get_name(){return video_name;}
        [[nodiscard]] size_t size() const { return frames_.size(); }
        [[nodiscard]] bool empty() const { return frames_.empty(); }
        const Frame &operator[](const size_t index) const { return *frames_[index]; }
        Frame &operator[](const size_t index) { return *frames_[index]; }

        //setter-esque methods
        void set_name(std::string name){video_name= std::move(name);}
        void push_frame(Frame f){frames_.push_back(std::make_shared<Frame>(std::move(f)));}


};

} // namespace vlite
