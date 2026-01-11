"""
Basic Vlite Usage Example
--------------------------
Demonstrates basic video loading, frame access, and sampling.
"""

import sys
sys.path.insert(0, '../build')  # Adjust path to your build directory
import vlite
import torch


def main():
    print("=" * 60)
    print("Vlite Basic Usage Example")
    print("=" * 60)

    # 1. Load a video
    print("\n1. Loading video...")
    video = vlite.Video()
    success = video.load("../test_files/test1.mp4", vlite.AV_PIX_FMT_RGB24)

    if not success:
        print("Error: Could not load video")
        return

    print(f"   Loaded {len(video)} frames")

    # 2. Access individual frames
    print("\n2. Accessing frames...")
    frame = video[0]
    print(f"   First frame: {frame.width}x{frame.height} @ {frame.timestamp}s")
    print(f"   Frame index: {frame.frame_index}")

    # Access middle and last frames
    mid_frame = video[len(video) // 2]
    last_frame = video[len(video) - 1]
    print(f"   Middle frame timestamp: {mid_frame.timestamp}s")
    print(f"   Last frame timestamp: {last_frame.timestamp}s")

    # 3. Convert single frame to tensor
    print("\n3. Converting frame to tensor...")
    frame_tensor = vlite.frame_to_tensor(video[0], normalize=False)
    print(f"   Tensor shape: {frame_tensor.shape}")  # [3, H, W]
    print(f"   Tensor dtype: {frame_tensor.dtype}")
    print(f"   Value range: [{frame_tensor.min().item()}, {frame_tensor.max().item()}]")

    # With normalization
    frame_tensor_norm = vlite.frame_to_tensor(video[0], normalize=True)
    print(f"   Normalized range: [{frame_tensor_norm.min().item():.3f}, "
          f"{frame_tensor_norm.max().item():.3f}]")

    # 4. Sample clips with different strategies
    print("\n4. Sampling clips...")

    # Uniform sampling
    print("   a) Uniform sampling:")
    uniform_sampler = vlite.UniformSampler()
    clips = uniform_sampler.sample_frames(video, num_clips=4, frames_per_clip=8)
    print(f"      Generated {len(clips)} clips")
    print(f"      Each clip has {len(clips[0].frames)} frames")

    # Random sampling
    print("   b) Random sampling (reproducible with seed):")
    random_sampler = vlite.RandomFrameSampler(seed=42)
    clips = random_sampler.sample_frames(video, num_clips=4, frames_per_clip=8)
    print(f"      Generated {len(clips)} clips")

    # Random segment sampling
    print("   c) Random segment sampling (consecutive frames):")
    segment_sampler = vlite.RandomSegmentSampler(seed=99)
    clips = segment_sampler.sample_frames(video, num_clips=2, frames_per_clip=16)
    print(f"      Generated {len(clips)} clips with {len(clips[0].frames)} consecutive frames")

    # 5. Convert clips to tensors
    print("\n5. Converting clips to tensors...")

    # Single clip
    clip_tensor = vlite.clip_to_tensor(clips[0], normalize=False)
    print(f"   Single clip shape: {clip_tensor.shape}")  # [T, C, H, W]

    # Multiple clips
    all_clips_tensor = vlite.clips_to_tensor(clips, normalize=True)
    print(f"   Multiple clips shape: {all_clips_tensor.shape}")  # [N, T, C, H, W]

    # Entire video
    print("\n6. Converting entire video to tensor...")
    print("   Warning: This may take time for long videos")
    video_tensor = vlite.video_to_tensor(video, normalize=True)
    print(f"   Full video tensor shape: {video_tensor.shape}")  # [T, C, H, W]

    print("\n" + "=" * 60)
    print("Basic usage example completed successfully!")
    print("=" * 60)


if __name__ == "__main__":
    main()
