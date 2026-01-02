import pytest
import vlite
import torch


def test_frame_to_tensor():
    video = vlite.Video()
    video.load("test_files/test1.mp4", vlite.AV_PIX_FMT_RGB24)
    tensor = vlite.frame_to_tensor(video[0], False)
    assert isinstance(tensor, torch.Tensor)
    assert tensor.dim() == 3
    assert tensor.size(0) == 3


def test_clip_to_tensor():
    video = vlite.Video()
    video.load("test_files/test1.mp4", vlite.AV_PIX_FMT_RGB24)
    sampler = vlite.UniformSampler()
    clips = sampler.sample_frames(video, 1, 4)
    tensor = vlite.clip_to_tensor(clips[0], False)
    assert isinstance(tensor, torch.Tensor)
    assert tensor.dim() == 4
    assert tensor.size(0) == 4


def test_tensor_normalization():
    video = vlite.Video()
    video.load("test_files/test1.mp4", vlite.AV_PIX_FMT_RGB24)
    tensor = vlite.frame_to_tensor(video[0], True)
    assert tensor.dtype == torch.float32
    assert tensor.min().item() >= 0.0
    assert tensor.max().item() <= 1.0
