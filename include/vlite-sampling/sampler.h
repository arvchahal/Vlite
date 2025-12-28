//
// Created by Arnav Chahal on 12/26/25.
//

#ifndef SAMPLER_H
#define SAMPLER_H
#include "../vlite-core/frame.h"
#include "../vlite-core/video.h"
#include <cmath>
namespace vlite {
/// frames in sequential order
struct Clip {
public:
  std::vector<std::shared_ptr<Frame>> frames;
};
inline std::ostream &operator<<(std::ostream &os, const Clip &c) {
  std::cout << "Frames in this clip" << &c.frames << std::endl;
  return os;
}

class Sampler {
public:
  virtual std::vector<std::shared_ptr<Clip>>
  sample_frames(Video *vid, int num_clips, int frames_per_clip) = 0;
  virtual ~Sampler() = default;

protected:
  Sampler() = default;
};
} // namespace vlite
#endif // SAMPLER_H
