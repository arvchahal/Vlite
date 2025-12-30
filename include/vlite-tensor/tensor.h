//
// Created by Arnav Chahal on 12/26/25.
//

#ifndef TENSOR_H
#define TENSOR_H
#include <torch/torch.h>
#include <vlite-sampling/sampler.h>
#include <vector>
#include "../vlite-core/frame.h"

namespace vlite {
    torch::Tensor frame_to_tensor(const Frame& frame, bool normalize = false);
    torch::Tensor clip_to_tensor(const Clip& clip, bool normalize = false);
    torch::Tensor clips_to_tensor(const std::vector<std::shared_ptr<Clip>>& clips, bool normalize = false);
    torch::Tensor video_to_tensor(const Video& video, bool normalize = false);
}
#endif //TENSOR_H
