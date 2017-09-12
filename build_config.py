#!python
import os
import os.path
import sys

build_base_dir = 'bin'
target_name = 'mip_upe_sdk'

# Scons does not have built-in support to create multiple object files at the same time.
# This helper fills that gap.


def create_objs(environ, SRCS):
  return [environ.Object(src) for src in SRCS]
