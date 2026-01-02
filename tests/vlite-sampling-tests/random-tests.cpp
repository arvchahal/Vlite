//
// Created by Arnav Chahal on 12/30/25.
//

#include <gtest/gtest.h>
#include <vlite-core/video.h>
#include <vlite-sampling/random.h>

extern "C" {
#include <libavutil/pixfmt.h>
}

TEST(RandomSamplerTest, BasicSampling) {
    vlite::Video video;
    ASSERT_TRUE(video.load("../test_files/test1.mp4", AV_PIX_FMT_RGB24));
    vlite::Random_Frame_Sampler sampler(42);
    auto clips = sampler.sample_frames(&video, 2, 4);
    EXPECT_EQ(clips.size(), 2);
    EXPECT_EQ(clips[0]->frames.size(), 4);
}

TEST(RandomSamplerTest, Deterministic) {
    vlite::Video video;
    ASSERT_TRUE(video.load("../test_files/test1.mp4", AV_PIX_FMT_RGB24));
    vlite::Random_Frame_Sampler sampler1(123);
    vlite::Random_Frame_Sampler sampler2(123);
    auto clips1 = sampler1.sample_frames(&video, 2, 3);
    auto clips2 = sampler2.sample_frames(&video, 2, 3);
    EXPECT_EQ(clips1[0]->frames[0]->frame_index, clips2[0]->frames[0]->frame_index);
}
