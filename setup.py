"""
Setup script for Vlite - Video Preprocessing SDK for Machine Learning
"""

import os
import sys
import subprocess
from pathlib import Path

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext


class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)


class CMakeBuild(build_ext):
    def run(self):
        try:
            subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError("CMake must be installed to build Vlite")

        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))

        # Create build directory
        build_temp = Path(self.build_temp)
        build_temp.mkdir(parents=True, exist_ok=True)

        cmake_args = [
            f'-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={extdir}',
            f'-DPYTHON_EXECUTABLE={sys.executable}',
            '-DCMAKE_BUILD_TYPE=Release',
        ]

        build_args = ['--config', 'Release']

        # Platform-specific options
        if sys.platform.startswith('darwin'):
            # macOS specific - requires 10.15+ for std::filesystem
            cmake_args += ['-DCMAKE_OSX_DEPLOYMENT_TARGET=10.15']

        # Call CMake
        subprocess.check_call(
            ['cmake', ext.sourcedir] + cmake_args,
            cwd=self.build_temp
        )

        # Build
        subprocess.check_call(
            ['cmake', '--build', '.', '--target', 'vlite_python'] + build_args,
            cwd=self.build_temp
        )


# Read long description from README
long_description = ""
readme_path = Path(__file__).parent / "README.md"
if readme_path.exists():
    long_description = readme_path.read_text(encoding='utf-8')


setup(
    name='vlite',
    version='0.1.0',
    author='Arnav Chahal',
    author_email='chahalarnav123@gmail.com',
    description='Modern C++20 video preprocessing SDK for machine learning',
    long_description=long_description,
    long_description_content_type='text/markdown',
    url='https://github.com/arvchahal/Vlite',
    project_urls={
        'Bug Tracker': 'https://github.com/arvchahal/Vlite/issues',
        'Documentation': 'https://github.com/arvchahal/Vlite/wiki',
        'Source Code': 'https://github.com/arvchahal/Vlite',
    },
    ext_modules=[CMakeExtension('vlite')],
    cmdclass={'build_ext': CMakeBuild},
    zip_safe=False,
    python_requires='>=3.9',
    install_requires=[
        'torch>=2.0.0',
        'numpy>=1.20.0',
    ],
    extras_require={
        'dev': [
            'pytest>=7.0.0',
            'pytest-cov>=3.0.0',
        ],
        'examples': [
            'pillow>=9.0.0',
        ],
    },
    classifiers=[
        'Development Status :: 4 - Beta',
        'Intended Audience :: Developers',
        'Intended Audience :: Science/Research',
        'Topic :: Scientific/Engineering :: Artificial Intelligence',
        'Topic :: Multimedia :: Video',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',
        'Programming Language :: Python :: 3.11',
        'Programming Language :: Python :: 3.12',
        'Programming Language :: C++',
        'Operating System :: POSIX :: Linux',
        'Operating System :: MacOS',
    ],
    keywords='video preprocessing machine-learning pytorch ffmpeg computer-vision',
)
