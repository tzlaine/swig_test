#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys, re

text_ = open(sys.argv[1]).read()

lines = []

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

    mauler = ''
    if '✛' in factors or '✚' in factors:
        mauler = ',\n%s    "mauler": "true"' % (indent)
        factors = factors.replace('✛', '').replace('✚', '')

    tug = ''
    if 'T' in factors:
        mauler = ',\n%s    "tug": "true"' % (indent)
        factors = factors.replace('T', '')

    fighters = ''
    heavy_fighter_bonus = ''
    match = cf_regex_1.match(factors)
    if match:
        fighter_factors = match.group(2)
        total = 0
        if fighter_factors.count('▲') == 1:
            total = 0.5
            fighter_factors = fighter_factors.replace('▲', '')
        # TODO: This doesn't seem to have done the trick for Fed heavies...
        hvy_ftr_match = hvy_ftr_regex.match(fighter_factors)
        if hvy_ftr_match:
            heavies = float(hvy_ftr_match.group(1))
            nominal_fighters = set_ == 'crippled' and 3.0 or 6.0
            heavy_fighter_bonus = \
                ',\n%s    "heavy fighter bonus": %s' % (indent, heavies - nominal_fighters)
            total += 6.0 + float(hvy_ftr_match.group(2))
        else:
            fighter_factors.replace('H', '')
            total += float(fighter_factors)
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

    return '%s"%s": {\n%s    "att": %s,\n%s    "def": %s%s%s%s%s%s%s%s\n%s},' % \
    (indent, set_, indent, attack, indent, defense, scout, escort, mauler, fighters, heavy_fighter_bonus, drones, flotillas, indent)


def stats (text):
    parts = text.split('/')
    retval = factors_set(parts[0], 'uncrippled')
    if not 'None' in parts[1]:
        retval += '\n' + factors_set(parts[1], 'crippled')
    return retval

avail_regex = re.compile(r'Y(\d+)([SF])?')

def available (field):
    match = avail_regex.match(field)
    year = match.group(1)
    season = 'spring'
    if match.group(2):
        season = match.group(2) == 'F' and 'fall' or 'spring'
    retval = indent + '"available": {\n' + \
             indent + '    "year": ' + str(int(year)) + ',\n' + \
             indent + '    "season": "' + season + '"\n' + \
             indent + '},'
    return retval

def print_pod_designation (field):
    if 'Pod' in field or 'Tug Mission' in field:
        print indent + '"pod": "true",'

def all_conversion_sources (sources):
    retval = []
    slash_split = sources.split('/')
    if 1 < len(slash_split):
        if len(slash_split[-1]) == 1:
            slash_split[-1] = slash_split[-2][0:-1] + slash_split[-1]
    for part in slash_split:
        if '?' in part:
            pattern = '^' + part.replace('?', '.') + '$'
            for line in lines:
                if re.match(pattern, line[0]):
                    retval.append(line[0])
        else:
            retval.append(part)
    return retval

no_conv_regex = re.compile(r'^(?:None|none|—NA—)$')
conv_regex = re.compile(r'[Ff]rom ([^:]+): ((?:[(][\d.]+[)])|(?:[\d+]+))')

def print_conversions (field):
    predefined_costs = {
        '(432.5)': '8' # CVB fighter surcharge
    }

    ignores = [
        '440.4' # From outside Basic F&E
    ]

    match = no_conv_regex.match(field)
    if match:
        return

    for ignore in ignores:
        if ignore in field:
            return

    retval = indent + '"conversions from": {'
    matches = conv_regex.findall(field)
    if len(matches) == 0:
        retval += '\n%s    "TODO": "%s"' % (indent, field)
    comma = ''
    for match in matches:
        if match[1] in predefined_costs:
            match = (match[0], predefined_costs[match[1]])
        converted_from = all_conversion_sources(match[0])
        cost = match[1].split('+')
        fighter_cost = ''
        if len(cost) == 3:
            cost = [int(cost[0]) + int(cost[1]), cost[2]]
        if len(cost) == 2:
            fighter_cost = ',\n%s        "fighter_cost": "%s"' % (indent, int(cost[1]))
        cost = int(cost[0])
        for convertee in converted_from:
            retval += \
                '%s\n%s    "%s": {\n%s        "cost": "%s"%s\n%s    }' % \
                (comma, indent, convertee, indent, cost, fighter_cost, indent)
            comma = ','
    print retval + '\n' + indent + '},'

def process_line (fields):
    outer_indent = '        '
    print outer_indent + '"' + fields[0] + '": {'
    print indent + '"cmd": ' + fields[4] + ','
    print stats(fields[2])
    print available(fields[5])
    print_pod_designation(fields[6])
    print_conversions(fields[7])
    print outer_indent + '},'

def save_line (fields):
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

    lines.append(fields)

line = ''
in_quote = False
for i in range(0, len(text_)):
    char = text_[i]
    if char == '"':
        in_quote = not in_quote
    if not in_quote or char != '\n':
        if char == '\n':
            line = line.replace('‡', '')
            save_line(line.split('\t'))
            line = ''
        else:
            line += char

for line in lines:
    process_line(line)
