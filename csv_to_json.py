#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys, re

text_ = open(sys.argv[1]).read()

indent = '            '

def normalized_name (name):
    paren = name.find(' (')
    if paren != -1:
        name = name[0:paren]
    return name

cf_regex_1 = re.compile(r'(.+)-(.+)(?:[(](.+)[)])?')
cf_regex_2 = re.compile(r'(.+)[(](.+)[)]')

def factors_set (factors, set_):
    flotillas = factors.count('P')
    if flotillas:
        flotillas = ',\n%s    "PFs": %d' % (indent, flotillas)
    else:
        flotillas = ''
    factors = factors.replace('P', '')
    scout = ''
    if '◆' in factors:
        scout = ',\n%s    "scout": "true"' % (indent)
        factors = factors.replace('◆', '')
    print 'regex1', cf_regex_1.match(factors)
    print 'regex2', cf_regex_2.match(factors)
    return '%s"%s": {\n%s    "att": %s,\n%s    "def": %s%s%s\n%s},' % \
    (indent, set_, indent, factors, indent, factors, scout, flotillas, indent)


def basic_factors (uncrippled, crippled):
    retval = factors_set(uncrippled, 'uncrippled')
    if not 'None' in crippled:
        retval += '\n' + factors_set(crippled, 'crippled')
    return retval

def combat_factors (text):
    parts = text.split('/')
#    print text, parts
#    match_1 = cf_regex_1.match(parts[0])
#    match_2 = cf_regex_2.match(parts[0])
#    if match_1:
#        return ''
#    elif match_2:
#        return ''
#    else:
    return basic_factors(parts[0], parts[1])

def process_line (fields):
    nonempty_fields = 0
    # strip quotes
    for i in range(0, len(fields)):
        field = fields[i]
        if 2 < len(field) and field[0] == '"' and field[-1] == '"':
            field = field[1:-1]
        fields[i] = field
        if 'Date Avail' in fields[i]:
            return;
        if fields[i] != '':
            nonempty_fields += 1

    if nonempty_fields < 5:
        return;

    outer_indent = '        '
    print outer_indent + '"' + normalized_name(fields[0]) + '": {'
    print indent + '"cmd": ' + fields[4] + ','
    print combat_factors(fields[2])
    print outer_indent + '},'

line = ''
in_quote = False
for i in range(0, len(text_)):
    char = text_[i]
    if char == '"':
        in_quote = not in_quote
    if not in_quote or char != '\n':
        if char == '\n':
            process_line(line.split('\t'))
            line = ''
        else:
            line += char
