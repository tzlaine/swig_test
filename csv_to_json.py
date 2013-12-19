#!/usr/bin/env python

import sys

lines = open(sys.argv[1]).readlines()

for line in lines:
    if 'F&E' in line:
        print line[0:-1]
