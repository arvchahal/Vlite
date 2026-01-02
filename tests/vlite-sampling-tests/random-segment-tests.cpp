//
// Created by Arnav Chahal on 12/30/25.
//

#include <gtest/gtest.h>
#include <vlite-core/video.h>
#include <vlite-sampling/randomsegment.h>

extern "C" {
#include <libavutil/pixfmt.h>
}

TEST(RandomSegmentSamplerTest, BasicSampling) {
    vlite::Video video;
    ASSERT_TRUE(video.load("../test_files/test1.mp4", AV_PIX_FMT_RGB24));
    vlite::Random_Segment_Sampler sampler(42);
    auto clips = sampler.sample_frames(&video, 2, 4);
    EXPECT_EQ(clips.size(), 2);
    EXPECT_EQ(clips[0]->frames.size(), 4);
}

TEST(RandomSegmentSamplerTest, FramesAreConsecutive) {
    vlite::Video video;
    ASSERT_TRUE(video.load("../test_files/test1.mp4", AV_PIX_FMT_RGB24));
    vlite::Random_Segment_Sampler sampler(99);
    auto clips = sampler.sample_frames(&video, 1, 4);
    for (size_t i = 1; i < clips[0]->frames.size(); ++i) {
        EXPECT_EQ(clips[0]->frames[i]->frame_index, clips[0]->frames[i-1]->frame_index + 1);
    }
}
