//
// Created by Arnav Chahal on 12/28/25.
//
#include "../../include/vlite-tensor/tensor.h"
#include <cstring>

namespace vlite {

// Shape: [C, H, W] (PyTorch convention: channels first)
torch::Tensor frame_to_tensor(const Frame& frame, bool normalize) {
    const int H = frame.height;
    const int W = frame.width;
    const int C = get_bytes_per_pixel(frame.format);  // 3 for RGB, 1 for grayscale

    // Create tensor with frame data (copying is necessary)
    auto options = torch::TensorOptions().dtype(torch::kUInt8);
    auto tensor = torch::from_blob(
        const_cast<uint8_t*>(frame.frameData.data()),
        {H, W, C},
        options
    ).clone();  // clone() copies data so tensor owns it

    tensor = tensor.permute({2, 0, 1});  // [H, W, C] → [C, H, W]

    if (normalize) {
        tensor = tensor.to(torch::kFloat32) / 255.0f;
    }

    return tensor;
}

// Shape: [T, C, H, W] where T = frames_per_clip
torch::Tensor clip_to_tensor(const Clip& clip, bool normalize) {
    if (clip.frames.empty()) {
        throw std::runtime_error("Cannot convert empty clip to tensor");
    }

    const int T = clip.frames.size();
    const int C = get_bytes_per_pixel(clip.frames[0]->format);
    const int H = clip.frames[0]->height;
    const int W = clip.frames[0]->width;

    auto dtype = normalize ? torch::kFloat32 : torch::kUInt8;
    auto tensor = torch::zeros({T, C, H, W}, torch::TensorOptions().dtype(dtype));

    for (int t = 0; t < T; t++) {
        tensor[t] = frame_to_tensor(*clip.frames[t], normalize);
    }

    return tensor;
}

// Shape: [N, T, C, H, W] where N = num_clips, T = frames_per_clip
torch::Tensor clips_to_tensor(const std::vector<std::shared_ptr<Clip>>& clips,
                               bool normalize) {
    if (clips.empty()) {
        throw std::runtime_error("Cannot convert empty clips vector to tensor");
    }

    const int N = clips.size();
    const int T = clips[0]->frames.size();
    const int C = get_bytes_per_pixel(clips[0]->frames[0]->format);
    const int H = clips[0]->frames[0]->height;
    const int W = clips[0]->frames[0]->width;

    auto dtype = normalize ? torch::kFloat32 : torch::kUInt8;
    auto tensor = torch::zeros({N, T, C, H, W}, torch::TensorOptions().dtype(dtype));

    for (int n = 0; n < N; n++) {
        tensor[n] = clip_to_tensor(*clips[n], normalize);
    }

    return tensor;
}

torch::Tensor video_to_tensor(const Video& video, bool normalize) {
    const auto& frames = video.get_frames();
    if (frames.empty()) {
        throw std::runtime_error("Cannot convert empty video to tensor");
    }

    const int T = frames.size();
    const int C = get_bytes_per_pixel(frames[0]->format);
    const int H = frames[0]->height;
    const int W = frames[0]->width;

    auto dtype = normalize ? torch::kFloat32 : torch::kUInt8;
    auto tensor = torch::zeros({T, C, H, W}, torch::TensorOptions().dtype(dtype));

    for (int t = 0; t < T; t++) {
        tensor[t] = frame_to_tensor(*frames[t], normalize);
    }

    return tensor;
}

} // namespace vlite
