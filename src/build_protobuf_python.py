#!/usr/bin/env python

import argparse
import subprocess
import sys
import os
import platform

parser = argparse.ArgumentParser(description='Build and install the Protobuf Python code.')
parser.add_argument('--working-dir', required=True, type=str,
                    help='The working directory in which to run the commands')
parser.add_argument('--protoc-path', required=True, type=str,
                    help='Path to the protoc executable')
parser.add_argument('--protobuf-python-dir', required=True, type=str,
                    help='The path into which to install the built Protobuf Python code')
args = parser.parse_args()

env = os.environ
if 'Windows' in platform.platform():
    env['PATH'] += ';' + args.protoc_path
else:
    env['PATH'] += ':' + args.protoc_path
subprocess.check_call([sys.executable, 'setup.py', 'build'], cwd=args.working_dir, env=env)

try:
    os.makedirs(args.protobuf_python_dir + '/Python27/site-packages')
except:
    pass

env = os.environ
env['PYTHONUSERBASE'] = args.protobuf_python_dir
subprocess.check_call([sys.executable, 'setup.py', 'install', '--user'], cwd=args.working_dir, env=env)
