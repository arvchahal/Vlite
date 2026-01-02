import pytest
import vlite


def test_video_creation():
    video = vlite.Video()
    assert video is not None


def test_video_load():
    video = vlite.Video()
    loaded = video.load("test_files/test1.mp4", vlite.AV_PIX_FMT_RGB24)
    assert loaded
    assert len(video) > 0


def test_video_indexing():
    video = vlite.Video()
    video.load("test_files/test1.mp4", vlite.AV_PIX_FMT_RGB24)
    frame = video[0]
    assert frame.width > 0
    assert frame.height > 0
    assert frame.frame_index == 0


def test_video_empty():
    video = vlite.Video()
    loaded = video.load("/nonexistent/fake.mp4", vlite.AV_PIX_FMT_RGB24)
    assert not loaded
