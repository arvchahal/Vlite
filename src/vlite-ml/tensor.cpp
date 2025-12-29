//
// Created by Arnav Chahal on 12/28/25.
//

#include "../../include/vlite-ml/tensor.h"
namespace vlite {

    torch::Tensor frame_to_tensor(const std::shared_ptr<Frame> & f) {
        const auto& data = f->frameData;
        const auto height = f->height;
        const auto width = f->width;
        const auto format = f->format;

        const auto byte_encoding = get_bytes_per_pixel(format);
        uint8_t arr[byte_encoding*width*height];
    }
    torch::Tensor clips_to_tensors(const std::vector<std::shared_ptr<Clip>> & clips) {

    }

    torch::Tensor clip_to_tensors(const std::shared_ptr<Clip> & clips) {

    }


}
