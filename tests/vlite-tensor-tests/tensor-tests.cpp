//
// Created by Arnav Chahal on 12/30/25.
//

#include <gtest/gtest.h>
#include <vlite-core/video.h>
#include <vlite-sampling/uniform.h>
#include <vlite-tensor/tensor.h>
#include <filesystem>
#include <iostream>

extern "C" {
#include <libavutil/pixfmt.h>
}

TEST(TensorTest, FrameToTensor) {
    std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;
    vlite::Video video;
    ASSERT_TRUE(video.load("../test_files/test1.mp4", AV_PIX_FMT_RGB24));
    auto tensor = vlite::frame_to_tensor(video[0], false);
    EXPECT_EQ(tensor.dim(), 3);
    EXPECT_EQ(tensor.size(0), 3);
}

TEST(TensorTest, ClipToTensor) {
    vlite::Video video;
    ASSERT_TRUE(video.load("../test_files/test1.mp4", AV_PIX_FMT_RGB24));
    vlite::Uniform_Spacing_Sampler sampler;
    auto clips = sampler.sample_frames(&video, 1, 4);
    auto tensor = vlite::clip_to_tensor(*clips[0], false);
    EXPECT_EQ(tensor.dim(), 4);
    EXPECT_EQ(tensor.size(0), 4);
}

TEST(TensorTest, ClipsToTensor) {
    vlite::Video video;
    ASSERT_TRUE(video.load("../test_files/test1.mp4", AV_PIX_FMT_RGB24));
    vlite::Uniform_Spacing_Sampler sampler;
    auto clips = sampler.sample_frames(&video, 2, 4);
    auto tensor = vlite::clips_to_tensor(clips, false);
    EXPECT_EQ(tensor.dim(), 5);
    EXPECT_EQ(tensor.size(0), 2);
}

TEST(TensorTest, NormalizedTensor) {
    vlite::Video video;
    ASSERT_TRUE(video.load("../test_files/test1.mp4", AV_PIX_FMT_RGB24));
    auto tensor = vlite::frame_to_tensor(video[0], true);
    EXPECT_EQ(tensor.dtype(), torch::kFloat32);
    auto min_val = tensor.min().item<float>();
    auto max_val = tensor.max().item<float>();
    EXPECT_GE(min_val, 0.0f);
    EXPECT_LE(max_val, 1.0f);
}

TEST(TensorTest, VideoToTensor) {
    vlite::Video video;
    ASSERT_TRUE(video.load("../test_files/test1.mp4", AV_PIX_FMT_RGB24));
    auto tensor = vlite::video_to_tensor(video, false);
    EXPECT_EQ(tensor.dim(), 4);
    EXPECT_EQ(tensor.size(0), video.size());
}
