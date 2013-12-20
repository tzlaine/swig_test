#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys, re

text_ = open(sys.argv[1]).read()

indent = '            '

def ship_group (name):
    return name.find(' (') != -1

cf_regex_1 = re.compile(r'(.+)[(](.+)[)]')
cf_regex_2 = re.compile(r'(.+)<(.+)>')
cf_regex_3 = re.compile(r'(.+)-(.+)')

hvy_ftr_regex = re.compile(r'(.+)H(.+)')

def factors_set (factors, set_):
    flotillas = factors.count('P')
    if flotillas:
        flotillas = ',\n%s    "PFs": %d' % (indent, flotillas * 6)
    else:
        flotillas = ''
    factors = factors.replace('P', '')

    scout = ''
    if '◆' in factors:
        scout = ',\n%s    "scout": "true"' % (indent)
        factors = factors.replace('◆', '')

    escort = ''
    if '■' in factors:
        escort = ',\n%s    "escort": "true"' % (indent)
        factors = factors.replace('■', '')

    fighters = ''
    match = cf_regex_1.match(factors)
    if match:
        fighter_factors = match.group(2)
        total = 0
        if fighter_factors.count('▲') == 1:
            total = 0.5
            fighter_factors = fighter_factors.replace('▲', '')
        ftr_match = hvy_ftr_regex.match(fighter_factors)
        if ftr_match:
            total += int(ftr_match.group(1)) + int(ftr_match.group(2))
        else:
            fighter_factors.replace('H', '')
            total += int(fighter_factors)
        fighters = ',\n%s    "fighters": %s' % (indent, total)
        factors = factors.replace('(' + match.group(2) + ')', '')

    drones = ''
    match = cf_regex_2.match(factors)
    if match:
        factors = match.group(1)
        drones = ',\n%s    "drones": %s' % (indent, match.group(2))

    attack = factors
    defense = factors

    match = cf_regex_3.match(factors)
    if match:
        attack = match.group(1)
        defense = match.group(2)

    return '%s"%s": {\n%s    "att": %s,\n%s    "def": %s%s%s%s%s%s\n%s},' % \
    (indent, set_, indent, attack, indent, defense, scout, escort, fighters, drones, flotillas, indent)


def basic_factors (uncrippled, crippled):
    retval = factors_set(uncrippled, 'uncrippled')
    if not 'None' in crippled:
        retval += '\n' + factors_set(crippled, 'crippled')
    return retval

def combat_factors (text):
    parts = text.split('/')
    return basic_factors(parts[0], parts[1])

def process_line (fields):
    nonempty_fields = 0
    # strip quotes
    for i in range(0, len(fields)):
        field = fields[i]
        if 2 < len(field) and field[0] == '"' and field[-1] == '"':
            field = field[1:-1]
        fields[i] = field
        if fields[3] != 'F&E':
            return
#        if 'Date Avail' in fields[i]:
#            return
#        if fields[i] != '':
#            nonempty_fields += 1
#
#    if nonempty_fields < 5:
#        return

    if ship_group(fields[0]):
        return

    outer_indent = '        '
    print outer_indent + '"' + fields[0] + '": {'
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
            line = line.replace('‡', '')
            process_line(line.split('\t'))
            line = ''
        else:
            line += char
