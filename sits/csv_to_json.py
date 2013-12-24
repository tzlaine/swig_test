#!/usr/bin/env python
# -*- coding: utf-8 -*-

import math, sys, re

text_ = open(sys.argv[1]).read()

nation = sys.argv[2]

lines = []

indent = '            '

ship_group_regex = re.compile(r' [(]\d')

def ship_group (name):
    return ship_group_regex.search(name)

cf_regex_1 = re.compile(r'(.+)[(](.+)[)]')
cf_regex_2 = re.compile(r'(.+)<(.+)>')
cf_regex_3 = re.compile(r'(.+)-(.+)')

hvy_ftr_regex = re.compile(r'(.+)H(.+)?')

def factors_set (factors, set_, tug_):
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
    if tug_ or 'T' in factors:
        all_missions = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'J1', 'J2', 'K1', 'K2', 'M', 'O']
        missions = '['
        for i in range(0, len(all_missions)):
            if True: # TODO: Limit tug missions allowed by unit.
                missions += '"' + all_missions[i] + '"'
                if i < len(all_missions) - 1:
                    missions += ', '
        missions += ']'
        mauler = ',\n%s    "tug missions": %s' % (indent, missions)
        factors = factors.replace('T', '')

    fighters = ''
    heavy_fighter_bonus = ''
    match = cf_regex_1.match(factors)
    if match:
        fighter_factors = match.group(2)
        total = 0
        hvy_ftr_match = hvy_ftr_regex.match(fighter_factors)
        if hvy_ftr_match:
            heavies = 0
            group_1 = hvy_ftr_match.group(1)
            group_2 = hvy_ftr_match.group(2)
            if group_1.count('▲') == 1:
                heavies += 0.5
                group_1 = group_1.replace('▲', '')
            heavies += float(group_1)
            nominal_fighter_squadron_size = set_ == 'crippled' and 3.0 or 6.0
            nominal_fighter_squadrons = \
              math.floor((heavies - 1.0) / nominal_fighter_squadron_size)
            nominal_fighters = nominal_fighter_squadrons * nominal_fighter_squadron_size
            heavy_fighter_bonus = \
                ',\n%s    "heavy fighter bonus": %s' % (indent, heavies - nominal_fighters)
            total += nominal_fighters
            if group_2:
                if group_2.count('▲') == 1:
                    total += 0.5
                group_2 = group_2.replace('▲', '')
                total += float(group_2)
        else:
            if fighter_factors.count('▲') == 1:
                total += 0.5
                fighter_factors = fighter_factors.replace('▲', '')
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

    return ('%s"%s": {\n%s    "att": %s,\n%s    "def": %s%s%s%s%s%s%s%s\n%s},' % \
    (indent, set_, indent, attack, indent, defense, scout, escort, mauler, fighters, heavy_fighter_bonus, drones, flotillas, indent), fighters != '')


def stats (text, tug):
    parts = text.split('/')
    (retval, fighters) = factors_set(parts[0], 'uncrippled', tug)
    if not 'None' in parts[1]:
        retval += '\n' + factors_set(parts[1], 'crippled', False)[0]
    return (retval, fighters)

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
    if 'Pod' in field or 'Pallet' in field or 'Tug Mission' in field:
        print indent + '"pod": "true",'
        return True
    return False

def all_sources (sources):
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

predefined_costs = {
    '(432.5)': '8' # CVB fighter surcharge
}

def cost (cost_str, extra_indent):
    cost_ = cost_str.split('+')
    fighter_cost_ = ''
    if len(cost_) == 3:
        cost_ = [int(cost_[0]) + int(cost_[1]), cost_[2]]
    if len(cost_) == 2:
        fighter_cost_ = ',\n%s%s"fighter_cost": %s' % (indent, extra_indent, int(cost_[1]))
    cost_ = int(cost_[0])
    return (cost_, fighter_cost_)

def subst_conv_costs (field, matches):
    retval = ''
    comma = ''
    for match in matches:
        if match[1] in predefined_costs:
            match = (match[0], predefined_costs[match[1]])
        sources = all_sources(match[0])
        (cost_, fighter_cost_) = cost(match[1], '        ')
        for source in sources:
            retval += \
                '%s\n%s    "%s": {\n%s        "cost": %s%s\n%s    }' % \
                (comma, indent, source, indent, cost_, fighter_cost_, indent)
            comma = ','
    return retval

cost_regex = re.compile(r'(?:[Ss]chedule|[Rr]eplacement|[(]431.22[)]): ([\d+]+)')
subst_regex = re.compile(r'[Ff]or (.+): ([\d+]+)')

def print_construction (field):
    #TODO: handle replacement-only units
    match = cost_regex.match(field)
    matches = subst_regex.findall(field)

    if match:
        constr = indent + '"construction": {'
        (cost_, fighter_cost_) = cost(match.group(1), '    ')
        constr += '\n%s    "cost": %s%s\n%s},' % \
        (indent, cost_, fighter_cost_, indent)
        print constr

    if 0 < len(matches):
        retval = indent + '"substitutions for": {'
        retval += subst_conv_costs(field, matches)
        print retval + '\n' + indent + '},'

no_conv_regex = re.compile(r'^(?:None|none|—NA—|NA)$')
conv_regex = re.compile(r'[Ff]rom ([^:]+): ((?:[(][\d.]+[)])|(?:[\d+]+))')

def print_conversions (field):
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
    retval += subst_conv_costs(field, matches)
    print retval + '\n' + indent + '},'

def print_salvage (field):
    if field != '' and float(field) != 0.0:
        print '%s"salvage": %d,' % (indent, float(field))

tug_regex = re.compile(r'[Tt]ug')
carrier_regex = re.compile(r'[Tt]rue *[Cc]arrier')
limit_regex = re.compile(r'(?:[Mm]ax|[Ll]imit) *(\d+)')

def get_notes (field):
    notes = 'none'
    if field != '':
        notes = field.strip()
    match = limit_regex.search(notes)
    limit = -1
    if match:
        limit = int(match.group(1))
    return (notes, tug_regex.match(notes), carrier_regex.search(notes), limit)

def print_notes (notes):
    print '%s"notes": "%s"' % (indent, notes)

def process_line (fields, last_line):
    (notes, tug, carrier, limit) = get_notes(fields[10])

    outer_indent = '        '
    print outer_indent + '"' + fields[0] + '": {'
    print indent + '"cmd": ' + fields[4] + ','
    (stats_, fighters) = stats(fields[2], tug)
    print stats_
    print available(fields[5])
    pod = print_pod_designation(fields[6])
    print_construction(fields[8])
    print_conversions(fields[7])

    move = 6
    if fields[0] == 'BATS' or fields[0] == 'BS' or fields[0] == 'MB (ND)' or fields[0] == 'MB' or fields[0] == 'PDU' or fields[0] == 'REPR' or pod:
        move = 0
    elif fields[0] == 'FRD':
        move = 1
    elif fields[0] == 'CONVOY':
        move = 2

    towable = False
    tow_move_cost = 1
    tow_strat_move_limit = -1
    if fields[0] == 'MB (ND)' or fields[0] == 'PDU':
        towable = True
    elif fields[0] == 'FRD':
        towable = True
        tow_move_cost = 2
        tow_strat_move_limit = 12

    spaceworthy = True
    if fields[0] == 'MB (ND)' or fields[0] == 'PDU' or pod:
        spaceworthy = False

    print '%s"move": %d,' % (indent, move)
    if move and (carrier or fighters and not nation == 'HYD'):
        print indent + '"carrier": "true",'
    if -1 < limit:
        print '%s"max in service": %d,' % (indent, limit)
    if towable:
        print '%s"towable": {\n%s    "move cost": %d,\n%s    "strat move limit": %d\n%s},' % \
        (indent, indent, tow_move_cost, indent, tow_strat_move_limit, indent)
    print_salvage(fields[9])
    if not spaceworthy:
        print indent + '"spaceworthy": false,'
    print_notes(notes)
    print outer_indent + '}' + (not last_line and ',' or '')

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

print '    "%s": {' % nation
for i in range(0, len(lines)):
    process_line(lines[i], i == len(lines) - 1)
print '    },'
