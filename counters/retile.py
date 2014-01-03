#!/usr/bin/python

import subprocess, sys

file_base = sys.argv[1]
columns = int(sys.argv[2])

args = ['montage', '-mode', 'concatenate', '-tile', '%dx7' % columns]
for i in range(0, 7):
    if columns == 1:
        args.append(file_base + ('_cut_%d.png' % i))
    else:
        args.append(file_base + ('_cut_%d_0.png' % i))
        args.append(file_base + ('_cut_%d_1.png' % i))
args.append(file_base + '_cut.png')
print args
subprocess.call(args)
