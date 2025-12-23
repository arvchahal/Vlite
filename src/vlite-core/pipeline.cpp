#include "../../include/vlite-core/pipeline.h"

namespace vlite {
    Pipeline::Pipeline(const std::string &video_path) {

    }

    bool Pipeline::resize(const int newWidth, const int newHeight, const AVPixelFormat newFormat) const {
        for (const auto& video: this->loaded_videos) {
            if (!video || video->empty()) {
                return false;
            }
            //rewrite as structured bindings
            auto peek_frame_ptr = video->get_frames()[0];
            if (!peek_frame_ptr){
                std::cout<<"shared pointer is null in peek frame"<<std::endl;
                return false;
            }
            Frame peek_frame = *peek_frame_ptr;
            auto oldWidth  = peek_frame.width, oldHeight = peek_frame.height;
            auto oldFormat = peek_frame.format;
            auto curr = *video;
            SwsContext *sws_ctx =
                sws_getContext(oldWidth, oldHeight, AVPixelFormat(oldFormat),
                        newWidth, newHeight, newFormat,
                        SWS_BILINEAR, nullptr, nullptr, nullptr);
            for (auto& frame_ptr: video->get_frames()) {
                if (!frame_ptr){
                    std::cout<<"shared pointer is null in all frames loop"<<std::endl;
                    return false;
                }
                Frame frame = *frame_ptr;
                std::vector<uint8_t> resized_data(newWidth * newHeight * 3);

                // Set up pointers (SwsContext wants arrays)
                const uint8_t* src_data[4] = {frame.frameData.data(), nullptr, nullptr, nullptr};
                int src_stride[4] = {frame.width * 3, 0, 0, 0};  // RGB24 stride = width * 3

                uint8_t* dst_data[4] = {resized_data.data(), nullptr, nullptr, nullptr};
                int dst_stride[4] = {newWidth * 3, 0, 0, 0};

                sws_scale(sws_ctx, src_data, src_stride, 0, frame.height,
                          dst_data, dst_stride);

                frame.frameData = std::move(resized_data);
                frame.width = newWidth;
                frame.height = newHeight;

            }
            sws_freeContext(sws_ctx);

        }
        return true;
    }

    bool Pipeline::load(const char* video_dir) {
        AVFormatContext *fmt_ctx = nullptr;
        for (const auto &video_path : std::filesystem::directory_iterator(video_dir)) {
            //converts the video_path from a directory_entry object to a char*
            std::string outfilename_str = video_path.path().string();
            const char* path = outfilename_str.c_str();
            Video temp;

            if (!temp.load(path,AV_PIX_FMT_RGB24)) {
                std::cout<< "Failed to load video";
                return false;
            }
            push_video(std::move(temp));



        }
        return true;
    }


}