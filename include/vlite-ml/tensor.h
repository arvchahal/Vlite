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
        torch::Tensor clips_to_tensors(const std::vector<std::shared_ptr<Clip>> & clips);
        torch::Tensor frame_to_tensor(const Frame *frame);
        torch::Tensor clip_to_tensors(const Clip *clip);

}

#endif //TENSOR_H
