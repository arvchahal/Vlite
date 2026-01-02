//
// Created by Arnav Chahal on 12/30/25.
//

#include <gtest/gtest.h>
#include <vlite-core/video.h>
#include <vlite-sampling/temporal.h>

extern "C" {
#include <libavutil/pixfmt.h>
}

TEST(TemporalSamplerTest, SampleAtZero) {
    vlite::Video video;
    ASSERT_TRUE(video.load("../test_files/test1.mp4", AV_PIX_FMT_RGB24));
    vlite::Timestamp_Sampler sampler;
    auto frame = sampler.sample_frame(&video, 0.0);
    ASSERT_NE(frame, nullptr);
    EXPECT_GE(frame->timestamp, 0.0);
}

TEST(TemporalSamplerTest, ReturnsFrame) {
    vlite::Video video;
    ASSERT_TRUE(video.load("../test_files/test1.mp4", AV_PIX_FMT_RGB24));
    vlite::Timestamp_Sampler sampler;
    auto frame = sampler.sample_frame(&video, 1.0);
    EXPECT_NE(frame, nullptr);
}
