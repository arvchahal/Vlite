//
// Created by Arnav Chahal on 12/30/25.
//

#include <gtest/gtest.h>
#include <vlite-core/pipeline.h>
#include <vlite-core/video.h>
#include <vlite-sampling/uniform.h>
#include <filesystem>

extern "C" {
#include <libavutil/pixfmt.h>
}

class PipelineTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_video1 = "../test_files/test1.mp4";
        test_video2 = "../test_files/test2.mp4";
        if (!std::filesystem::exists(test_video1)) {
            GTEST_SKIP();
        }
    }
    std::string test_video1;
    std::string test_video2;
};

TEST_F(PipelineTest, PushVideo) {
    vlite::Pipeline pipeline;
    vlite::Video video;
    ASSERT_TRUE(video.load(test_video1.c_str(), AV_PIX_FMT_RGB24));
    pipeline.push_video(std::move(video));
    EXPECT_EQ(pipeline.get_videos().size(), 1);
}

TEST_F(PipelineTest, SampleAll) {
    vlite::Pipeline pipeline;
    vlite::Video video;
    ASSERT_TRUE(video.load(test_video1.c_str(), AV_PIX_FMT_RGB24));
    pipeline.push_video(std::move(video));

    vlite::Uniform_Spacing_Sampler sampler;
    auto tensors = pipeline.sample_all(&sampler, 2, 4, false);

    ASSERT_EQ(tensors.size(), 1);
    EXPECT_EQ(tensors[0].dim(), 5);
    EXPECT_EQ(tensors[0].size(0), 2);
    EXPECT_EQ(tensors[0].size(1), 4);
}

TEST_F(PipelineTest, SampleAllNormalized) {
    vlite::Pipeline pipeline;
    vlite::Video video;
    ASSERT_TRUE(video.load(test_video1.c_str(), AV_PIX_FMT_RGB24));
    pipeline.push_video(std::move(video));

    vlite::Uniform_Spacing_Sampler sampler;
    auto tensors = pipeline.sample_all(&sampler, 1, 4, true);

    EXPECT_EQ(tensors[0].dtype(), torch::kFloat32);
    EXPECT_GE(tensors[0].min().item<float>(), 0.0f);
    EXPECT_LE(tensors[0].max().item<float>(), 1.0f);
}
