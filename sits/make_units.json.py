#!/usr/bin/env python

import subprocess

files = [
    ('msit_feds.csv', 'FED'),
    ('msit_gorns.csv', 'GOR'),
    ('msit_hydrans.csv', 'HYD'),
    ('msit_klingon.csv', 'KLI'),
    ('msit_kzinti.csv', 'KZI'),
    ('msit_lyran.csv', 'LYR'),
    ('msit_orion.csv', 'ORI'),
    ('msit_rom.csv', 'ROM'),
    ('msit_tholian.csv', 'THO')
]

generic_units = subprocess.check_output(['./csv_to_json.py', 'generic.csv', 'GENERIC'])[0:-1];
open('generic.units', 'w').write(generic_units)

print '{'
for i in range(0, len(files)):
    output = subprocess.check_output(['./csv_to_json.py', files[i][0], files[i][1]]);
    if i == len(files) - 1:
        print output[0:-2]
    else:
        print output[0:-1]
print '}'
