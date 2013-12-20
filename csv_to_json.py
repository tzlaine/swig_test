#!/usr/bin/env python

import sys #, re

text_ = open(sys.argv[1]).read()

def process_line (fields):
    # strip quotes
    for i in range(0, len(fields)):
        field = fields[i]
        if field[0] == '"' and field[-1] == '"':
            fields[i] = field[1:-1]

    print fields

line = ''
in_quote = False
for i in range(0, len(text_)):
    char = text_[i]
    if char == '"':
        in_quote = not in_quote
    if not in_quote or char != '\n':
        if char == '\n':
            if 'F&E' in line:
                process_line(line.split('\t'))
            line = ''
        else:
            line += char
