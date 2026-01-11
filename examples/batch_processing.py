"""
Batch Video Processing Example
-------------------------------
Demonstrates using Vlite Pipeline for efficient batch processing
of multiple videos.
"""

import sys
sys.path.insert(0, '../build')
import vlite

import torch
import time
from pathlib import Path


def process_videos_individually(video_paths, sampler, num_clips, frames_per_clip):
    """
    Process videos one at a time (slower approach).
    """
    print("\n1. Processing videos individually...")
    start_time = time.time()

    all_tensors = []
    for i, video_path in enumerate(video_paths):
        video = vlite.Video()
        video.load(str(video_path), vlite.AV_PIX_FMT_RGB24)

        clips = sampler.sample_frames(video, num_clips, frames_per_clip)
        tensor = vlite.clips_to_tensor(clips, normalize=True)
        all_tensors.append(tensor)

        print(f"   Processed video {i + 1}/{len(video_paths)}: {tensor.shape}")

    elapsed = time.time() - start_time
    print(f"   Time: {elapsed:.2f}s")

    return all_tensors


def process_videos_with_pipeline(video_dir, sampler, num_clips, frames_per_clip):
    """
    Process videos using Pipeline (faster approach).
    """
    print("\n2. Processing videos with Pipeline...")
    start_time = time.time()

    pipeline = vlite.Pipeline()
    pipeline.load(str(video_dir), vlite.AV_PIX_FMT_RGB24)

    print(f"   Loaded {pipeline.num_videos()} videos")

    # Process all videos at once
    tensors = pipeline.sample_all(sampler, num_clips, frames_per_clip, normalize=True)

    for i, tensor in enumerate(tensors):
        print(f"   Video {i + 1}: {tensor.shape}")

    elapsed = time.time() - start_time
    print(f"   Time: {elapsed:.2f}s")

    return tensors


def process_videos_custom(video_dir, num_clips, frames_per_clip):
    """
    Custom processing: different sampling strategy per video.
    """
    print("\n3. Custom processing (different samplers per video)...")

    pipeline = vlite.Pipeline()
    pipeline.load(str(video_dir), vlite.AV_PIX_FMT_RGB24)

    samplers = [
        vlite.UniformSampler(),
        vlite.RandomFrameSampler(seed=42),
        vlite.RandomSegmentSampler(seed=99)
    ]

    for i in range(pipeline.num_videos()):
        video = pipeline.get_video(i)
        sampler = samplers[i % len(samplers)]

        clips = sampler.sample_frames(video, num_clips, frames_per_clip)
        tensor = vlite.clips_to_tensor(clips, normalize=True)

        sampler_name = type(sampler).__name__
        print(f"   Video {i + 1} ({sampler_name}): {tensor.shape}")


def create_training_batch(video_dir, batch_size=4):
    """
    Create batches suitable for training.
    """
    print(f"\n4. Creating training batches (batch_size={batch_size})...")

    pipeline = vlite.Pipeline()
    pipeline.load(str(video_dir), vlite.AV_PIX_FMT_RGB24)

    # Sample all videos
    sampler = vlite.UniformSampler()
    tensors = pipeline.sample_all(sampler, num_clips=4, frames_per_clip=16, normalize=True)

    # Stack into batches
    num_videos = len(tensors)
    num_batches = (num_videos + batch_size - 1) // batch_size

    print(f"   Total videos: {num_videos}")
    print(f"   Batches: {num_batches}")

    for batch_idx in range(num_batches):
        start_idx = batch_idx * batch_size
        end_idx = min(start_idx + batch_size, num_videos)

        batch_tensors = tensors[start_idx:end_idx]

        # Stack tensors into a batch
        # Each tensor: [num_clips, frames_per_clip, C, H, W]
        if len(batch_tensors) > 0:
            # Pad batch if necessary
            while len(batch_tensors) < batch_size and batch_idx < num_batches - 1:
                batch_tensors.append(torch.zeros_like(batch_tensors[0]))

            batch = torch.stack(batch_tensors)
            print(f"   Batch {batch_idx + 1}: {batch.shape}")  # [batch, num_clips, T, C, H, W]


def export_frames(video_path, output_dir, every_n_frames=10):
    """
    Export frames from video as individual images.
    """
    print(f"\n5. Exporting frames from video...")

    video = vlite.Video()
    video.load(str(video_path), vlite.AV_PIX_FMT_RGB24)

    output_path = Path(output_dir)
    output_path.mkdir(exist_ok=True)

    exported = 0
    for i in range(0, len(video), every_n_frames):
        frame = video[i]
        tensor = vlite.frame_to_tensor(frame, normalize=False)

        # Convert to PIL Image and save
        # (requires PIL: pip install pillow)
        try:
            from PIL import Image
            import numpy as np

            # Convert tensor [C, H, W] to numpy [H, W, C]
            img_array = tensor.permute(1, 2, 0).numpy().astype('uint8')
            img = Image.fromarray(img_array)
            img.save(output_path / f"frame_{i:06d}.jpg")
            exported += 1
        except ImportError:
            print("   PIL not installed, skipping image export")
            break

    if exported > 0:
        print(f"   Exported {exported} frames to {output_dir}")


def main():
    print("=" * 60)
    print("Batch Video Processing with Vlite")
    print("=" * 60)

    VIDEO_DIR = Path("../test_files")

    # Get all video files
    video_paths = list(VIDEO_DIR.glob("*.mp4"))

    if len(video_paths) == 0:
        print(f"\nNo videos found in {VIDEO_DIR}")
        print("Please add some .mp4 files to test with.")
        return

    print(f"\nFound {len(video_paths)} videos in {VIDEO_DIR}")

    # Configuration
    NUM_CLIPS = 4
    FRAMES_PER_CLIP = 8

    sampler = vlite.UniformSampler()

    # Compare different processing approaches
    if len(video_paths) > 1:
        tensors1 = process_videos_individually(
            video_paths[:2],  # Use first 2 videos for comparison
            sampler,
            NUM_CLIPS,
            FRAMES_PER_CLIP
        )

        tensors2 = process_videos_with_pipeline(
            VIDEO_DIR,
            sampler,
            NUM_CLIPS,
            FRAMES_PER_CLIP
        )

        print("\n   Pipeline is more efficient for batch processing!")

    # Custom processing
    process_videos_custom(VIDEO_DIR, NUM_CLIPS, FRAMES_PER_CLIP)

    # Training batches
    create_training_batch(VIDEO_DIR, batch_size=2)

    # Export frames (optional - requires PIL)
    if len(video_paths) > 0:
        export_frames(video_paths[0], "./exported_frames", every_n_frames=30)

    print("\n" + "=" * 60)
    print("Batch processing example completed!")
    print("=" * 60)


if __name__ == "__main__":
    main()
