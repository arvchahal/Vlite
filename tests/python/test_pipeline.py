import pytest
import vlite
import torch


def test_pipeline_creation():
    pipeline = vlite.Pipeline()
    assert pipeline is not None
    assert pipeline.num_videos() == 0


def test_pipeline_push_video():
    pipeline = vlite.Pipeline()
    video = vlite.Video()
    video.load("test_files/test1.mp4", vlite.AV_PIX_FMT_RGB24)
    pipeline.push_video(video)
    assert pipeline.num_videos() == 1


def test_pipeline_sample_all():
    pipeline = vlite.Pipeline()
    video = vlite.Video()
    video.load("test_files/test1.mp4", vlite.AV_PIX_FMT_RGB24)
    pipeline.push_video(video)

    sampler = vlite.UniformSampler()
    tensors = pipeline.sample_all(sampler, 2, 4, False)

    assert len(tensors) == 1
    assert isinstance(tensors[0], torch.Tensor)
    assert tensors[0].shape[0] == 2
    assert tensors[0].shape[1] == 4


def test_pipeline_sample_all_normalized():
    pipeline = vlite.Pipeline()
    video = vlite.Video()
    video.load("test_files/test1.mp4", vlite.AV_PIX_FMT_RGB24)
    pipeline.push_video(video)

    sampler = vlite.UniformSampler()
    tensors = pipeline.sample_all(sampler, 1, 4, True)

    assert tensors[0].dtype == torch.float32
