import argparse

parser = argparse.ArgumentParser(description='Takes a file with a newline-delimited list of names, and turns it into a string table suitable for use by UE5..')
parser.add_argument('names_file', type=argparse.FileType('r', encoding='utf-8'),
                    help='Newline-delimited list of names')
args = parser.parse_args()

lines = args.names_file.readlines()

print('Key,SourceString')
for line in lines:
    line = line[:-1]
    print(f'"{line}","{line}"')

