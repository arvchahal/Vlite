# Vlite - Video Preprocessing SDK for Machine Learning

A modern C++20 video preprocessing library designed specifically for machine learning workflows. Vlite provides a clean, minimal interface for loading videos, decoding frames, performing transformations, sampling, and converting directly into PyTorch tensors.

## Features

- **Video Loading & Decoding**: Load single videos or entire directories using FFmpeg
- **Frame Transformations**: Resize, rescale, and convert pixel formats (RGB, YUV, Grayscale)
- **Temporal Sampling**: Uniform, random, random-segment, and timestamp-based sampling
- **ML Integration**: Direct conversion to PyTorch tensors with normalization support
- **Python Bindings**: Clean Python API via pybind11
- **Batch Processing**: Pipeline support for processing multiple videos

## Installation

### Prerequisites

- C++20 compatible compiler (GCC 10+, Clang 12+, MSVC 2019+)
- CMake 3.28+
- FFmpeg libraries (libavcodec, libavformat, libavutil, libswscale)
- Python 3.9+ (for Python bindings)
- PyTorch 2.0+ (for tensor conversion)

### Ubuntu/Debian

```bash
sudo apt-get update
sudo apt-get install -y ffmpeg libavcodec-dev libavformat-dev \
    libavutil-dev libswscale-dev libswresample-dev libavfilter-dev \
    libavdevice-dev pkg-config cmake build-essential
```

### macOS

```bash
brew install ffmpeg pkg-config cmake
```

### Build from Source

```bash
# Clone the repository
git clone https://github.com/yourusername/Vlite.git
cd Vlite

# Install pybind11
git clone https://github.com/pybind/pybind11.git $HOME/libs/pybind11

# Create Python virtual environment
python3 -m venv env
source env/bin/activate
pip install torch numpy pytest

# Build
cmake -B build -S .
cmake --build build

# Run tests
./build/vlite_tests  # C++ tests
PYTHONPATH=build pytest tests/python/  # Python tests
```

## Quick Start

### Python API

```python
import sys
sys.path.insert(0, 'build')
import vlite
import torch

# Load a video
video = vlite.Video()
video.load("path/to/video.mp4", vlite.AV_PIX_FMT_RGB24)

print(f"Loaded {len(video)} frames")

# Access individual frames
frame = video[0]
print(f"Frame size: {frame.width}x{frame.height}")

# Sample frames
sampler = vlite.UniformSampler()
clips = sampler.sample_frames(video, num_clips=4, frames_per_clip=16)

# Convert to PyTorch tensor
tensor = vlite.clip_to_tensor(clips[0], normalize=True)
print(f"Tensor shape: {tensor.shape}")  # [16, 3, H, W]
```

### Batch Processing with Pipeline

```python
import vlite

# Load multiple videos
pipeline = vlite.Pipeline()
pipeline.load("path/to/video_directory/", vlite.AV_PIX_FMT_RGB24)

# Sample all videos at once
sampler = vlite.UniformSampler()
tensors = pipeline.sample_all(sampler, num_clips=2, frames_per_clip=8, normalize=True)

# Each tensor is shape [num_clips, frames_per_clip, C, H, W]
for i, tensor in enumerate(tensors):
    print(f"Video {i}: {tensor.shape}")
```

## Sampling Strategies

### Uniform Sampling
Evenly spaced frames across the video:

```python
sampler = vlite.UniformSampler()
clips = sampler.sample_frames(video, num_clips=4, frames_per_clip=16)
```

### Random Frame Sampling
Randomly selected frames (with optional seed for reproducibility):

```python
sampler = vlite.RandomFrameSampler(seed=42)
clips = sampler.sample_frames(video, num_clips=4, frames_per_clip=16)
```

### Random Segment Sampling
Consecutive frames from random segments:

```python
sampler = vlite.RandomSegmentSampler(seed=99)
clips = sampler.sample_frames(video, num_clips=4, frames_per_clip=16)
```

### Timestamp Sampling
Sample frame at specific time:

```python
sampler = vlite.TimestampSampler()
frame = sampler.sample_frame(video, seconds=10.5)
```

## Tensor Conversion

Convert frames and clips to PyTorch tensors:

```python
# Single frame -> [C, H, W]
tensor = vlite.frame_to_tensor(video[0], normalize=False)

# Single clip -> [T, C, H, W]
tensor = vlite.clip_to_tensor(clip, normalize=True)

# Multiple clips -> [N, T, C, H, W]
tensor = vlite.clips_to_tensor(clips, normalize=True)

# Entire video -> [T, C, H, W]
tensor = vlite.video_to_tensor(video, normalize=False)
```

When `normalize=True`, pixel values are converted to float32 and scaled to [0, 1].

## Available Pixel Formats

```python
vlite.AV_PIX_FMT_RGB24      # 24-bit RGB
vlite.AV_PIX_FMT_GRAY8      # 8-bit Grayscale
vlite.AV_PIX_FMT_YUV420P    # YUV 4:2:0
```

## Available Codecs (for saving videos)

```python
vlite.AV_CODEC_ID_H264      # H.264/AVC
vlite.AV_CODEC_ID_H265      # H.265/HEVC
vlite.AV_CODEC_ID_VP9       # VP9
```

## Examples

See the `examples/` directory for complete examples:

- `examples/basic_usage.py` - Simple video loading and frame access
- `examples/train_classifier.py` - Video classification training loop
- `examples/batch_processing.py` - Processing multiple videos with Pipeline

## C++ API

```cpp
#include <vlite-core/video.h>
#include <vlite-sampling/uniform.h>
#include <vlite-tensor/tensor.h>

// Load video
vlite::Video video;
video.load("path/to/video.mp4", AV_PIX_FMT_RGB24);

// Sample frames
vlite::Uniform_Spacing_Sampler sampler;
auto clips = sampler.sample_frames(&video, 4, 16);

// Convert to tensor
auto tensor = vlite::clip_to_tensor(*clips[0], false);
```

## Architecture

```
Vlite/
├── src/
│   ├── vlite-core/       # Video loading, Frame handling
│   ├── vlite-sampling/   # Sampling strategies
│   └── vlite-tensor/     # Tensor conversion
├── include/              # Public headers
├── vlite-pybind/        # Python bindings
├── tests/               # C++ and Python tests
└── examples/            # Usage examples
```

## Performance Tips

1. **Use batch processing**: `Pipeline.sample_all()` is more efficient than processing videos individually
2. **Choose appropriate sampling**: Uniform sampling is faster than random for large videos
3. **Normalize only when needed**: Set `normalize=False` if your model expects uint8
4. **Reuse samplers**: Sampler objects are stateless and can be reused

## License

MIT License - see LICENSE file for details

## Contributing

Contributions welcome! Please:

1. Fork the repository
2. Create a feature branch
3. Add tests for new features
4. Ensure all tests pass
5. Submit a pull request

## Citation

If you use Vlite in your research, please cite:

```bibtex
@software{vlite2025,
  title={Vlite: Video Preprocessing SDK for Machine Learning},
  author={Your Name},
  year={2025},
  url={https://github.com/yourusername/Vlite}
}
```

## Support

- Issues: [GitHub Issues](https://github.com/yourusername/Vlite/issues)
- Documentation: [Wiki](https://github.com/yourusername/Vlite/wiki)
