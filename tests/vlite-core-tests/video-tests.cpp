//
// Created by Arnav Chahal on 12/30/25.
//

#include <gtest/gtest.h>
#include <vlite-core/video.h>
#include <filesystem>

extern "C" {
#include <libavutil/pixfmt.h>
}

class VideoTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_video1 = "../test_files/test1.mp4";
        test_video2 = "../test_files/test2.mp4";
        if (!std::filesystem::exists(test_video1)) {
            GTEST_SKIP() << "Test video not found: " << test_video1;
        }
    }
    std::string test_video1;
    std::string test_video2;
};

TEST_F(VideoTest, LoadValidVideo) {
    vlite::Video video;
    ASSERT_TRUE(video.load(test_video1.c_str(), AV_PIX_FMT_RGB24));
    EXPECT_GT(video.size(), 0);
    EXPECT_FALSE(video.empty());
}

TEST_F(VideoTest, LoadInvalidPath) {
    vlite::Video video;
    EXPECT_FALSE(video.load("/nonexistent/fake.mp4", AV_PIX_FMT_RGB24));
    EXPECT_TRUE(video.empty());
}

TEST_F(VideoTest, FrameAccess) {
    vlite::Video video;
    ASSERT_TRUE(video.load(test_video1.c_str(), AV_PIX_FMT_RGB24));
    const auto& frame = video[0];
    EXPECT_EQ(frame.frame_index, 0);
    EXPECT_GT(frame.width, 0);
    EXPECT_GT(frame.height, 0);
}

TEST_F(VideoTest, PixelFormat) {
    vlite::Video video;
    ASSERT_TRUE(video.load(test_video1.c_str(), AV_PIX_FMT_RGB24));
    const auto& frame = video[0];
    EXPECT_EQ(frame.format, AV_PIX_FMT_RGB24);
    EXPECT_EQ(frame.frameData.size(), frame.width * frame.height * 3);
}
