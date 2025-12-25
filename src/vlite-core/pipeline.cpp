#include "../../include/vlite-core/pipeline.h"

namespace vlite {
    Pipeline::Pipeline(const std::string &video_path) {

    }

    bool Pipeline::resize(const int newWidth, const int newHeight, const AVPixelFormat newFormat) {
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
            const Frame& peek_frame = *peek_frame_ptr;
            auto oldWidth  = peek_frame.width, oldHeight = peek_frame.height;
            auto oldFormat = peek_frame.format;
            if (!video->resize( oldFormat, oldWidth, oldHeight, newFormat, newWidth, newHeight)) {
                std::cout << "Failed to resize video" << std::endl;
                return false;
            }
        }
        return true;
    }

    bool Pipeline::load(const char* video_dir, AVPixelFormat format) {
        AVFormatContext *fmt_ctx = nullptr;
        for (const auto &video_path : std::filesystem::directory_iterator(video_dir)) {
            //converts the video_path from a directory_entry object to a char*
            std::string outfilename_str = video_path.path().string();
            const char* path = outfilename_str.c_str();
            Video temp;

            if (!temp.load(path,format)) {
                std::cout<< "Failed to load video";
                return false;
            }
            push_video(std::move(temp));



        }
        return true;
    }

    bool Pipeline::load_resize(const char *video_dirpath, int newWidth, int newHeight, AVPixelFormat newFormat) {
        int new_bpp = get_bytes_per_pixel(newFormat);
        if (new_bpp < 0) {
            std::cerr << "Unsupported output format in resize\n";
            return false;
        }

        for (const auto& dir_obj:std::filesystem::directory_iterator(video_dirpath)) {
            const char * vid_path = dir_obj.path().c_str();
            Video video;
            if(!video.load_resize(vid_path,newFormat,newWidth,newHeight)) {
                std::cerr<<"Error loading video from video path: "<<vid_path<<std::endl;
                return false;
            }
            push_video(std::move(video));
        }
        return true;
    }

    bool Pipeline::save(const char* video_dir_path, AVPixelFormat) {
        std::filesystem::create_directory(video_dir_path);

        for (const auto& vid_ptr: loaded_videos) {
            std::string full_path = std::string(video_dir_path) + vid_ptr->get_name();

            if (!vid_ptr->save(full_path.c_str())) {
                return false;
            }

        }
        return true;
    }




}