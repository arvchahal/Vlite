#!/usr/bin/env python3
"""
Test script to verify Vlite installation from TestPyPI

Usage:
    # First, install from TestPyPI
    pip install -i https://test.pypi.org/simple/ --extra-index-url https://pypi.org/simple/ vlite

    # Then run this test
    python test_installation.py
"""

import sys
import tempfile
import os
from pathlib import Path

def test_import():
    """Test that the package can be imported"""
    print("Testing import...")
    try:
        import vlite
        print("✓ Successfully imported vlite")
        return True
    except ImportError as e:
        print(f"✗ Failed to import vlite: {e}")
        return False

def test_video_class():
    """Test that Video class is available"""
    print("\nTesting Video class availability...")
    try:
        import vlite
        video = vlite.Video()
        print(f"✓ Video class instantiated: {type(video)}")
        return True
    except Exception as e:
        print(f"✗ Failed to instantiate Video: {e}")
        return False

def test_samplers():
    """Test that sampler classes are available"""
    print("\nTesting sampler classes...")
    try:
        import vlite

        samplers = [
            ("UniformSampler", vlite.UniformSampler),
            ("RandomFrameSampler", vlite.RandomFrameSampler),
            ("RandomSegmentSampler", vlite.RandomSegmentSampler),
            ("TimestampSampler", vlite.TimestampSampler),
        ]

        for name, sampler_class in samplers:
            sampler = sampler_class()
            print(f"  ✓ {name} instantiated")

        return True
    except Exception as e:
        print(f"✗ Failed to test samplers: {e}")
        return False

def test_tensor_functions():
    """Test that tensor conversion functions are available"""
    print("\nTesting tensor conversion functions...")
    try:
        import vlite

        functions = [
            "frame_to_tensor",
            "clip_to_tensor",
            "clips_to_tensor",
            "video_to_tensor",
        ]

        for func_name in functions:
            if hasattr(vlite, func_name):
                print(f"  ✓ {func_name} is available")
            else:
                print(f"  ✗ {func_name} is missing")
                return False

        return True
    except Exception as e:
        print(f"✗ Failed to test tensor functions: {e}")
        return False

def test_pipeline():
    """Test Pipeline class"""
    print("\nTesting Pipeline class...")
    try:
        import vlite
        pipeline = vlite.Pipeline()
        print(f"✓ Pipeline class instantiated: {type(pipeline)}")
        return True
    except Exception as e:
        print(f"✗ Failed to instantiate Pipeline: {e}")
        return False

def test_pixel_formats():
    """Test that pixel format constants are available"""
    print("\nTesting pixel format constants...")
    try:
        import vlite

        formats = [
            "AV_PIX_FMT_RGB24",
            "AV_PIX_FMT_GRAY8",
            "AV_PIX_FMT_YUV420P",
        ]

        for fmt in formats:
            if hasattr(vlite, fmt):
                print(f"  ✓ {fmt} = {getattr(vlite, fmt)}")
            else:
                print(f"  ✗ {fmt} is missing")
                return False

        return True
    except Exception as e:
        print(f"✗ Failed to test pixel formats: {e}")
        return False

def test_codec_constants():
    """Test that codec constants are available"""
    print("\nTesting codec constants...")
    try:
        import vlite

        codecs = [
            "AV_CODEC_ID_H264",
            "AV_CODEC_ID_H265",
            "AV_CODEC_ID_VP9",
        ]

        for codec in codecs:
            if hasattr(vlite, codec):
                print(f"  ✓ {codec} = {getattr(vlite, codec)}")
            else:
                print(f"  ✗ {codec} is missing")
                return False

        return True
    except Exception as e:
        print(f"✗ Failed to test codecs: {e}")
        return False

def test_dependencies():
    """Test that required dependencies are available"""
    print("\nTesting dependencies...")
    try:
        import torch
        print(f"  ✓ PyTorch {torch.__version__}")

        import numpy
        print(f"  ✓ NumPy {numpy.__version__}")

        return True
    except ImportError as e:
        print(f"✗ Failed to import dependency: {e}")
        return False

def main():
    """Run all tests"""
    print("=" * 60)
    print("Vlite Installation Test Suite")
    print("=" * 60)

    tests = [
        ("Import", test_import),
        ("Video Class", test_video_class),
        ("Samplers", test_samplers),
        ("Tensor Functions", test_tensor_functions),
        ("Pipeline", test_pipeline),
        ("Pixel Formats", test_pixel_formats),
        ("Codecs", test_codec_constants),
        ("Dependencies", test_dependencies),
    ]

    results = []
    for name, test_func in tests:
        try:
            result = test_func()
            results.append((name, result))
        except Exception as e:
            print(f"\n✗ Test '{name}' crashed: {e}")
            results.append((name, False))

    print("\n" + "=" * 60)
    print("Test Results Summary")
    print("=" * 60)

    passed = sum(1 for _, result in results if result)
    total = len(results)

    for name, result in results:
        status = "✓ PASS" if result else "✗ FAIL"
        print(f"{status}: {name}")

    print(f"\nPassed: {passed}/{total}")

    if passed == total:
        print("\n🎉 All tests passed! Vlite is properly installed.")
        return 0
    else:
        print(f"\n❌ {total - passed} test(s) failed.")
        return 1

if __name__ == "__main__":
    sys.exit(main())
