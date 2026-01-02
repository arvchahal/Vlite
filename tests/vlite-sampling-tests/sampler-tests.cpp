//
// Created by Arnav Chahal on 12/30/25.
//

#include <gtest/gtest.h>
#include <vlite-core/video.h>
#include <vlite-sampling/uniform.h>

extern "C" {
#include <libavutil/pixfmt.h>
}

TEST(SamplerTest, ClipsNotEmpty) {
    vlite::Video video;
    ASSERT_TRUE(video.load("../test_files/test1.mp4", AV_PIX_FMT_RGB24));
    vlite::Uniform_Spacing_Sampler sampler;
    auto clips = sampler.sample_frames(&video, 1, 4);
    ASSERT_FALSE(clips.empty());
    EXPECT_FALSE(clips[0]->frames.empty());
}
