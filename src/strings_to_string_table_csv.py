import argparse

parser = argparse.ArgumentParser(description='Takes a file with a newline-delimited list of names, and turns it into a string table suitable for use by UE5..')
parser.add_argument('names_file', type=argparse.FileType('r', encoding='utf-8'),
                    help='Newline-delimited list of names')
parser.add_argument('-o', required=True, type=argparse.FileType('w', encoding='utf-8'),
                    help='CSV file to write')
args = parser.parse_args()

lines = args.names_file.readlines()

args.o.write('Key,SourceString\n')
for line in lines:
    line = line[:-1]
    args.o.write(f'"{line}","{line}"\n')

