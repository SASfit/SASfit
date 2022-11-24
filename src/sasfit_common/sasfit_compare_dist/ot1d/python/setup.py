# -*- coding: utf-8 -*-
#
# @fileoverview Copyright (c) 2019-2021, Stefano Gualandi,
#                via Ferrata, 1, I-27100, Pavia, Italy
#
#  @author stefano.gualandi@gmail.com (Stefano Gualandi)
#

from setuptools import setup, Extension, Command, find_packages
from Cython.Build import cythonize

import numpy as np
import platform

CC_ARGS = [
    '-Wno-unused-function', '-std=c++11', '-O2', '-ffast-math',
    '-DNDEBUG', '-fno-wrapv', '-march=native'
]

LD_ARGS = ['-O2', '-lm', '-pthread', '-fno-wrapv']

if platform.system() == 'Windows':
    # Compiler options: 
    # https://docs.microsoft.com/en-us/cpp/build/reference/openmp-enable-openmp-2-0-support
    CC_ARGS = ['/std:c++14', '/arch:AVX2', '/O2']
    LD_ARGS = []

if platform.system() == 'Darwin':
    CC_ARGS = [
        '-std=c++11', '-stdlib=libc++', '-O2', '-ffast-math', '-DNDEBUG'
    ]
    LD_ARGS = ['-O2', '-lm', '-pthread', '-fno-wrapv']


extensions = Extension(name="OT1D",
                       sources=["OT1D.pyx"],
                       include_dirs=['./'],
                       extra_compile_args=CC_ARGS,
                       extra_link_args=LD_ARGS)

with open('README.md', encoding="utf-8") as f:
    long_descr = f.read()

setup(
    name='OT1D',
    version='0.3.6',
    packages=find_packages(),
    description='Discrete Optimal Transport in 1D by Linear Programming',
    author='Stefano Gualandi',
    author_email='stefano.gualandi@gmail.com',
    url='https://github.com/stegua/ot1d',
    platforms=['linux', 'macosx', 'windows'],
    setup_requires=['numpy', 'cython'],
    install_requires=['numpy', 'cython'],
    include_dirs=np.get_include(),
    long_description=long_descr,
    long_description_content_type='text/markdown',
    ext_modules = cythonize(extensions, 
        compiler_directives={'language_level': "3"})
)
