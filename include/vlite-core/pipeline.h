#include "./video.h"
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <vector>
#include <torch/torch.h>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

namespace vlite {
class Sampler; // Forward declaration

class Pipeline {
  std::vector<std::unique_ptr<Video>> loaded_videos;

public:
  Pipeline() = default;
  explicit Pipeline(const std::string &videos_path);
  [[nodiscard]] bool resize(int newWidth, int newHeight,
                            AVPixelFormat newFormat);
  [[nodiscard]] bool load(const char *video_dirpath, AVPixelFormat format);
  [[nodiscard]] bool load_resize(const char *video_dirpath, int newWidth,
                                 int newHeight, AVPixelFormat newFormat);
  [[nodiscard]] bool save(const char *video_dir_path, AVPixelFormat format);

  // Batch sampling: applies sampler to all loaded videos and returns tensors
  std::vector<torch::Tensor> sample_all(Sampler* sampler, int num_clips,
                                        int frames_per_clip, bool normalize = false);

  void push_video(Video vid) {
    loaded_videos.push_back(std::make_unique<Video>(std::move(vid)));
  }
  std::vector<std::unique_ptr<Video>>& get_videos() { return loaded_videos; }
};
} // namespace vlite
