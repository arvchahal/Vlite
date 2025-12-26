//
// Created by Arnav Chahal on 12/26/25.
//

#ifndef UNIFORM_H
#define UNIFORM_H
#include "./sampler.h"
namespace vlite {
/// samples frames deterministcially spaced evenly apart if the frames per clip
/// is > than the number of frames in the vide then we will be returning the
/// desired numnber of clips but with max frames per clips and likely not
/// uniformly spaced unless we are somehow lucky and num_frames / num_clips ==
/// int
class Uniform_Spacing_Sampler : Sampler {
public:
  Uniform_Spacing_Sampler() = default;
  std::vector<std::shared_ptr<Clip>>
  sample_frames( Video *vid, int num_clips,
    int frames_per_clip) override;
};
} // namespace vlite
#endif // UNIFORM_H
