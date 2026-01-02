import sys
import os

# Add build directory to Python path
build_dir = os.path.join(os.path.dirname(__file__), '..', '..', 'build')
sys.path.insert(0, build_dir)
