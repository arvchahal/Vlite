#include "./video.h"
#include <memory>
#include <vector>
#include <string>
#include <tuple>
#include <filesystem>
#include <iostream>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}
namespace vlite {
  class Pipeline {
    std::vector<std::shared_ptr<Video>> loaded_videos;
  public:
    Pipeline() = default;
    explicit Pipeline(const std::string &videos_path);
    [[nodiscard]] bool resize(int newWidth, int newHeight, AVPixelFormat newFormat);
    [[nodiscard]] bool load(const char* video_dirpath, AVPixelFormat format);
    [[nodiscard]] bool load_resize( const char *video_dirpath, int newWidth, int newHeight, AVPixelFormat newFormat);
    [[nodiscard]] bool save(const char* video_dir_path, AVPixelFormat format);

    void push_video(Video vid){loaded_videos.push_back(std::make_shared<Video>(std::move(vid)));}
    std::shared_ptr<Video> operator[](int index){return loaded_videos[index];}
  };
}