//
// Created by Arnav Chahal on 12/30/25.
//

#include <gtest/gtest.h>
#include <vlite-core/video.h>
#include <vlite-core/frame.h>
#include <filesystem>

extern "C" {
#include <libavutil/pixfmt.h>
}

class FrameTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_video = "../test_files/test1.mp4";
        if (!std::filesystem::exists(test_video)) {
            GTEST_SKIP();
        }
    }
    std::string test_video;
};

TEST_F(FrameTest, BasicProperties) {
    vlite::Video video;
    ASSERT_TRUE(video.load(test_video.c_str(), AV_PIX_FMT_RGB24));
    const auto& frame = video[0];
    EXPECT_GT(frame.width, 0);
    EXPECT_GT(frame.height, 0);
    EXPECT_GE(frame.timestamp, 0.0);
    EXPECT_EQ(frame.frame_index, 0);
}

TEST_F(FrameTest, DataSize) {
    vlite::Video video;
    ASSERT_TRUE(video.load(test_video.c_str(), AV_PIX_FMT_RGB24));
    const auto& frame = video[0];
    EXPECT_EQ(frame.frameData.size(), frame.width * frame.height * 3);
}
