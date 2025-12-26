//
// Created by Arnav Chahal on 12/26/25.
//

#ifndef SAMPLER_H
#define SAMPLER_H
#include "../vlite-core/frame.h"
#include "../vlite-core/video.h"
namespace vlite {
/// frames in sequential order
struct Clip {
  std::vector<std::shared_ptr<Frame>> frames;
};
class Sampler {
public:
  virtual std::vector<std::shared_ptr<Clip>>
  sample_frames(Video *vid, int num_clips, int frames_per_clip) = 0;
  virtual ~Sampler() = default;

protected:
  Sampler();
};
} // namespace vlite
#endif // SAMPLER_H
