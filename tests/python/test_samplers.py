import pytest
import vlite


def test_uniform_sampler():
    video = vlite.Video()
    video.load("test_files/test1.mp4", vlite.AV_PIX_FMT_RGB24)
    sampler = vlite.UniformSampler()
    clips = sampler.sample_frames(video, 2, 4)
    assert len(clips) == 2
    assert len(clips[0].frames) == 4


def test_random_sampler():
    video = vlite.Video()
    video.load("test_files/test1.mp4", vlite.AV_PIX_FMT_RGB24)
    sampler = vlite.RandomFrameSampler(42)
    clips = sampler.sample_frames(video, 2, 3)
    assert len(clips) == 2


def test_random_segment_sampler():
    video = vlite.Video()
    video.load("test_files/test1.mp4", vlite.AV_PIX_FMT_RGB24)
    sampler = vlite.RandomSegmentSampler(99)
    clips = sampler.sample_frames(video, 1, 4)
    assert len(clips) == 1
