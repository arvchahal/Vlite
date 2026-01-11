# Vlite Examples

This directory contains practical examples demonstrating how to use Vlite for video preprocessing in ML workflows.

## Examples

### 1. Basic Usage (`basic_usage.py`)

Learn the fundamentals:
- Loading videos
- Accessing individual frames
- Different sampling strategies
- Converting to PyTorch tensors

```bash
cd examples
python basic_usage.py
```

### 2. Video Classification Training (`train_classifier.py`)

Complete training pipeline example:
- Custom PyTorch Dataset using Vlite
- 3D CNN model for video classification
- Full training loop with batching
- Data loading and preprocessing

```bash
python train_classifier.py
```

**Key Features:**
- Custom `VideoDataset` class
- Configurable sampling strategies
- Mini-batch training
- Model checkpointing

### 3. Batch Processing (`batch_processing.py`)

Efficient batch video processing:
- Individual vs Pipeline processing comparison
- Different samplers per video
- Creating training batches
- Exporting frames to images

```bash
python batch_processing.py
```

**Performance Tips Demonstrated:**
- Pipeline is faster for multiple videos
- Batch processing techniques
- Memory-efficient processing

## Requirements

All examples require:
```bash
pip install torch numpy
```

For frame export in `batch_processing.py`:
```bash
pip install pillow
```

## Adapting Examples

### Using Your Own Videos

Update the video paths in each example:

```python
# Change this
VIDEO_DIR = "../test_files"

# To your video directory
VIDEO_DIR = "/path/to/your/videos"
```

### Customizing Sampling

All examples support different sampling strategies:

```python
# Uniform spacing (default)
sampler = vlite.UniformSampler()

# Random frames (with seed for reproducibility)
sampler = vlite.RandomFrameSampler(seed=42)

# Random consecutive segments
sampler = vlite.RandomSegmentSampler(seed=99)
```

### Adjusting Clip Parameters

Tune for your use case:

```python
NUM_CLIPS = 4           # Number of clips per video
FRAMES_PER_CLIP = 16    # Frames per clip (temporal dimension)
```

**Guidelines:**
- Action recognition: 16-32 frames/clip
- Short gestures: 8-16 frames/clip
- Long videos: More clips with fewer frames

## Integration with Popular Models

### SlowFast Networks

```python
# SlowFast needs two pathways with different frame rates
slow_sampler = vlite.UniformSampler()
slow_clips = slow_sampler.sample_frames(video, num_clips=1, frames_per_clip=8)

# For fast pathway, sample every other frame
# (implement custom sampling or use more frames)
```

### I3D / C3D

```python
# Standard configuration for I3D
sampler = vlite.UniformSampler()
clips = sampler.sample_frames(video, num_clips=1, frames_per_clip=64)
tensor = vlite.clip_to_tensor(clips[0], normalize=True)
```

### TimeSformer / ViViT (Transformer-based)

```python
# Transformers often use more frames
sampler = vlite.UniformSampler()
clips = sampler.sample_frames(video, num_clips=1, frames_per_clip=32)
```

## Performance Benchmarks

Typical processing times on M1 Mac:

| Operation | Time (approx) |
|-----------|---------------|
| Load 200-frame video | ~1-2 seconds |
| Sample 4 clips × 16 frames | ~0.5 seconds |
| Convert to tensor | ~0.1 seconds |
| Pipeline (10 videos) | ~15-20 seconds |

## Troubleshooting

### ImportError: No module named 'vlite'

Make sure the build directory is in your Python path:
```python
import sys
sys.path.insert(0, '../build')  # Adjust path as needed
```

### Video loading fails

Check:
1. FFmpeg is installed and working
2. Video file is not corrupted
3. Video codec is supported
4. File path is correct

### Out of memory

For large videos:
- Reduce `num_clips` or `frames_per_clip`
- Process videos individually instead of batching
- Use `normalize=False` to keep uint8 dtype

## Next Steps

- Check the [main README](../README.md) for API documentation
- Read the [C++ examples](../src/) for low-level usage
- Join discussions on [GitHub](https://github.com/yourusername/Vlite/discussions)
