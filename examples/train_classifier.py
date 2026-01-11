"""
Video Classification Training Example
--------------------------------------
Demonstrates how to use Vlite in a PyTorch training loop
for video classification (e.g., action recognition).
"""

import sys
sys.path.insert(0, '../build')
import vlite

import torch
import torch.nn as nn
import torch.optim as optim
from torch.utils.data import Dataset, DataLoader
import os
from pathlib import Path


class VideoDataset(Dataset):
    """
    PyTorch Dataset for video classification using Vlite.
    """
    def __init__(self, video_dir, num_clips=4, frames_per_clip=16,
                 normalize=True, sampler_type='uniform'):
        """
        Args:
            video_dir: Directory containing video files
            num_clips: Number of clips to sample from each video
            frames_per_clip: Number of frames per clip
            normalize: Whether to normalize pixel values to [0, 1]
            sampler_type: 'uniform', 'random', or 'segment'
        """
        self.video_paths = list(Path(video_dir).glob('*.mp4'))
        self.num_clips = num_clips
        self.frames_per_clip = frames_per_clip
        self.normalize = normalize

        # Create sampler
        if sampler_type == 'uniform':
            self.sampler = vlite.UniformSampler()
        elif sampler_type == 'random':
            self.sampler = vlite.RandomFrameSampler()
        elif sampler_type == 'segment':
            self.sampler = vlite.RandomSegmentSampler()
        else:
            raise ValueError(f"Unknown sampler type: {sampler_type}")

    def __len__(self):
        return len(self.video_paths)

    def __getitem__(self, idx):
        """
        Returns:
            clips: Tensor of shape [num_clips, frames_per_clip, C, H, W]
            label: Integer label (dummy for this example)
        """
        video_path = str(self.video_paths[idx])

        # Load video
        video = vlite.Video()
        success = video.load(video_path, vlite.AV_PIX_FMT_RGB24)

        if not success:
            # Return dummy data on failure
            return torch.zeros(self.num_clips, self.frames_per_clip, 3, 224, 224), 0

        # Sample clips
        clips = self.sampler.sample_frames(
            video,
            self.num_clips,
            self.frames_per_clip
        )

        # Convert to tensor [num_clips, frames_per_clip, C, H, W]
        clips_tensor = vlite.clips_to_tensor(clips, self.normalize)

        # Dummy label (in real scenario, you'd have actual labels)
        label = idx % 10  # 10 classes

        return clips_tensor, label


class Simple3DCNN(nn.Module):
    """
    Simple 3D CNN for video classification.
    """
    def __init__(self, num_classes=10, num_clips=4, frames_per_clip=16):
        super().__init__()

        self.num_clips = num_clips

        # 3D Convolutional layers
        self.conv1 = nn.Conv3d(3, 64, kernel_size=(3, 3, 3), padding=(1, 1, 1))
        self.pool1 = nn.MaxPool3d(kernel_size=(1, 2, 2), stride=(1, 2, 2))

        self.conv2 = nn.Conv3d(64, 128, kernel_size=(3, 3, 3), padding=(1, 1, 1))
        self.pool2 = nn.MaxPool3d(kernel_size=(2, 2, 2), stride=(2, 2, 2))

        self.conv3 = nn.Conv3d(128, 256, kernel_size=(3, 3, 3), padding=(1, 1, 1))
        self.pool3 = nn.MaxPool3d(kernel_size=(2, 2, 2), stride=(2, 2, 2))

        # Global average pooling
        self.gap = nn.AdaptiveAvgPool3d((1, 1, 1))

        # Fully connected layer
        self.fc = nn.Linear(256, num_classes)

        self.relu = nn.ReLU()

    def forward(self, x):
        """
        Args:
            x: [batch, num_clips, frames_per_clip, C, H, W]
        Returns:
            logits: [batch, num_classes]
        """
        batch_size = x.shape[0]

        # Merge batch and clips: [batch * num_clips, C, frames_per_clip, H, W]
        x = x.view(-1, 3, self.frames_per_clip, x.shape[-2], x.shape[-1])

        # Rearrange to [batch * num_clips, C, T, H, W] for Conv3d
        x = self.relu(self.conv1(x))
        x = self.pool1(x)

        x = self.relu(self.conv2(x))
        x = self.pool2(x)

        x = self.relu(self.conv3(x))
        x = self.pool3(x)

        # Global average pooling
        x = self.gap(x)
        x = x.view(batch_size, self.num_clips, -1)

        # Average across clips
        x = x.mean(dim=1)

        # Classification
        logits = self.fc(x)

        return logits


def train_one_epoch(model, dataloader, criterion, optimizer, device):
    """Train for one epoch."""
    model.train()
    running_loss = 0.0
    correct = 0
    total = 0

    for batch_idx, (clips, labels) in enumerate(dataloader):
        clips = clips.to(device)
        labels = labels.to(device)

        # Forward pass
        optimizer.zero_grad()
        outputs = model(clips)
        loss = criterion(outputs, labels)

        # Backward pass
        loss.backward()
        optimizer.step()

        # Statistics
        running_loss += loss.item()
        _, predicted = outputs.max(1)
        total += labels.size(0)
        correct += predicted.eq(labels).sum().item()

        if (batch_idx + 1) % 10 == 0:
            print(f"  Batch {batch_idx + 1}/{len(dataloader)} | "
                  f"Loss: {running_loss / (batch_idx + 1):.3f} | "
                  f"Acc: {100. * correct / total:.2f}%")

    epoch_loss = running_loss / len(dataloader)
    epoch_acc = 100. * correct / total

    return epoch_loss, epoch_acc


def main():
    print("=" * 60)
    print("Video Classification Training with Vlite")
    print("=" * 60)

    # Configuration
    VIDEO_DIR = "../test_files"  # Directory with your videos
    NUM_CLIPS = 4
    FRAMES_PER_CLIP = 16
    BATCH_SIZE = 2
    NUM_EPOCHS = 3
    LEARNING_RATE = 0.001
    NUM_CLASSES = 10

    # Device
    device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
    print(f"\nUsing device: {device}")

    # Create dataset
    print(f"\nCreating dataset from {VIDEO_DIR}...")
    dataset = VideoDataset(
        video_dir=VIDEO_DIR,
        num_clips=NUM_CLIPS,
        frames_per_clip=FRAMES_PER_CLIP,
        normalize=True,
        sampler_type='uniform'
    )
    print(f"Found {len(dataset)} videos")

    # Create dataloader
    dataloader = DataLoader(
        dataset,
        batch_size=BATCH_SIZE,
        shuffle=True,
        num_workers=0  # Set to 0 for debugging, increase for speed
    )

    # Create model
    print("\nInitializing model...")
    model = Simple3DCNN(
        num_classes=NUM_CLASSES,
        num_clips=NUM_CLIPS,
        frames_per_clip=FRAMES_PER_CLIP
    ).to(device)

    # Loss and optimizer
    criterion = nn.CrossEntropyLoss()
    optimizer = optim.Adam(model.parameters(), lr=LEARNING_RATE)

    # Training loop
    print(f"\nTraining for {NUM_EPOCHS} epochs...")
    print("=" * 60)

    for epoch in range(NUM_EPOCHS):
        print(f"\nEpoch {epoch + 1}/{NUM_EPOCHS}")
        print("-" * 60)

        loss, acc = train_one_epoch(model, dataloader, criterion, optimizer, device)

        print(f"Epoch {epoch + 1} Summary: Loss = {loss:.3f}, Accuracy = {acc:.2f}%")

    print("\n" + "=" * 60)
    print("Training completed!")
    print("=" * 60)

    # Save model
    torch.save(model.state_dict(), 'video_classifier.pth')
    print("\nModel saved to video_classifier.pth")


if __name__ == "__main__":
    main()
