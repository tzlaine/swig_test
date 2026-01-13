#!/usr/bin/env python

lines = open('cultures.txt').readlines()

culture_names = []
culture_groups = {}

names_tags = ['male_names', 'female_names', 'dynasty_names']

def tag(l):
    l = l.strip()
    return l[:-4]

def cleaned_up_names(text):
    retval = []
    text = text.replace('\t', ' ')
    tokens = text.split(' ')
    accum_tok = ''
    for tok in tokens:
        if tok.startswith('"'):
            accum_tok = tok
        elif tok.endswith('"'):
            accum_tok += ' ' + tok
            retval.append(accum_tok)
            accum_tok = ''
        elif accum_tok != '':
            accum_tok += ' ' + tok
        elif tok != '':
            retval.append('"' + tok + '"')
    return retval

gathering = []
gathered_text = ''
for l in lines:
    if '#' in l:
        l = l.split('#')[0]
    if '{' in l and '}' in l:
        continue
    if '}' in l:
        # print('CLOSE', gathering)
        if len(gathering) == 2 and gathering[1] in names_tags:
            # print(f'saving to {gathering[0]} {gathering[1]}: {cleaned_up_names(gathered_text)}')
            culture_groups[gathering[0]][gathering[1]] = cleaned_up_names(gathered_text)
        if len(gathering) == 3:
            culture_groups[gathering[0]][gathering[1]][gathering[2]] = cleaned_up_names(gathered_text)
        gathering = gathering[:-1]
        gathered_text = ''
        # print('  gathering=', gathering)
        continue
    if '{' in l:
        t = tag(l)
        culture = None
        if t not in names_tags:
            culture = t
            if '_' not in culture:
                culture_names.append("'" + culture.title() + "'")
        if culture != None:
            if len(gathering) == 0: # cuture group
                culture_groups[culture] = {}
            elif len(gathering) == 1: # cuture sub-group
                culture_groups[gathering[0]][culture] = {}
        gathering.append(t)
        # print('OPEN', gathering)
        # print('  gathering=', gathering)
        continue
    if ' = ' not in l:
        # print('GATHER', l)
        gathered_text += l.strip() + ' '
        # print('  gathering=', gathering)

print(f'-- WARNING!  This file is auto-generated.  Changes here will likely be lost.\n\n')
print(f'culture_names = {{\n   {',\n   '.join(culture_names)}\n}}\n')

general_tag_names = {
    'male_names': 'general_male_first_names',
    'female_names': 'general_female_first_names',
    'dynasty_names': 'general_last_names'
}
subculture_tag_names = {
    'male_names': 'male_first_names',
    'female_names': 'female_first_names',
    'dynasty_names': 'last_names'
}

def print_names(names, tag_mapping, indent):
    for tag in names_tags:
        if tag in names and 0 < len(names[tag]):
            print(f'{' '  * indent * 3}{tag_mapping[tag]} = {{')
            print(f'{' '  * indent * 3}   {', '.join(names[tag])}')
            print(f'{' '  * indent * 3}}},')
        else:
            print(f'{' '  * indent * 3}{tag_mapping[tag]} = {{}},')

def empty(x):
    for tag in names_tags:
        if tag in x and 0 < len(x[tag]):
            return False
    return True

def all_empty(x):
    if not empty(x):
        return False
    for _, sub in names.items():
        if not empty(sub):
            return False
    return True

print('cultures = {')
for culture, names in culture_groups.items():
    if culture != 'british':
        pass #continue
    if all_empty(names):
        continue
    print('   {')
    print_names(names, general_tag_names, 2)
    print('      subcultures = {')
    for subculture, sub_names in names.items():
        if empty(sub_names):
            continue
        print('         {')
        print_names(sub_names, subculture_tag_names, 4)
        print('         },')
    print('      },')
    print('   },')
print('}')
