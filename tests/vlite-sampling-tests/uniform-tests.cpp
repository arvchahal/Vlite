//
// Created by Arnav Chahal on 12/30/25.
//

#include <gtest/gtest.h>
#include <vlite-core/video.h>
#include <vlite-sampling/uniform.h>
#include <filesystem>
#include <iostream>

extern "C" {
#include <libavutil/pixfmt.h>
}

TEST(UniformSamplerTest, BasicSampling) {
    std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;
    vlite::Video video;
    ASSERT_TRUE(video.load("../test_files/test1.mp4", AV_PIX_FMT_RGB24));
    vlite::Uniform_Spacing_Sampler sampler;
    auto clips = sampler.sample_frames(&video, 2, 4);
    EXPECT_EQ(clips.size(), 2);
    EXPECT_EQ(clips[0]->frames.size(), 4);
}

TEST(UniformSamplerTest, ReturnsCorrectClipCount) {
    vlite::Video video;
    ASSERT_TRUE(video.load("../test_files/test1.mp4", AV_PIX_FMT_RGB24));
    vlite::Uniform_Spacing_Sampler sampler;
    auto clips = sampler.sample_frames(&video, 3, 5);
    EXPECT_EQ(clips.size(), 3);
}
