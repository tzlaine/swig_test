#!/usr/bin/env python

import re
import argparse
import os

parser = argparse.ArgumentParser(description='Generate C++ structs and binding layers from Protobuf messages.')
parser.add_argument('--in-file', required=True, type=argparse.FileType('r'),
                    help='Input .proto file')
parser.add_argument('--cpp-file', required=True, type=argparse.FileType('w'),
                    help='Output .cpp file')
parser.add_argument('--hpp-file', required=True, type=argparse.FileType('w'),
                    help='Output .hpp file')
args = parser.parse_args()

contents = args.in_file.read()

# Using 'void' for several of these that are not yet supported, to cause
# compilation failures in the generated types.
to_cpp_type = {
    'double': 'double',
    'float': 'float',
    'int32': 'int',
    'int64': 'void',
    'uint32': 'unsigned int',
    'uint64': 'void',
    'sint32': 'int',
    'sint64': 'void',
    'fixed32': 'int',
    'fixed64': 'void',
    'sfixed32': 'int',
    'sfixed64': 'void',
    'bool': 'bool',
    'string': 'std::string',
    'bytes': 'void',
}

def get_cpp_type(typename):
    if typename in to_cpp_type:
        typename = to_cpp_type[typename]
    return typename

def get_value_type(type_or_types):
    if type(type_or_types) is tuple:
        return type_or_types[1]
    else:
        return type_or_types

enums = []
enum_names = []
messages = []
message_names = []

enum_p = re.compile(r'enum +([^ {]+) *{([^}]+)}', re.DOTALL)
enumerator_p = re.compile(r'([^ ]+) *= *([^ ])+')
for enum in enum_p.findall(contents):
    name = enum[0].strip()
    enumerators = filter(lambda x: x, map(lambda x: x.strip(), enum[1].split(';')))
    enumerators = map(lambda x: enumerator_p.findall(x)[0], enumerators)
    enums.append((name, enumerators))
    enum_names.append(name)

message_p = re.compile(r'message +([^ {]+) *{([^}]+)}', re.DOTALL)
map_member_p = re.compile(r'map *< *([^ ]+), *([^ ]+) *> *([^ ]+) *= *')
member_p = re.compile(r'([^ ]+) *([^ ]+) *= *[^ ]+')
for message in message_p.findall(contents):
    name = message[0].strip()
    members_ = filter(lambda x: x, map(lambda x: x.strip(), message[1].split(';')))
    message_names.append(name)
    members = []
    for member in members_:
        match = map_member_p.search(member)
        repeated = 'repeated' in member
        if match:
            members.append((repeated, (match.group(1), match.group(2)), match.group(3)))
        else:
            match = member_p.search(member)
            members.append((repeated, match.group(1), match.group(2)))
            message_names.append(name)
    messages.append((name, members))

args.hpp_file.write('''#pragma once

// WARNING: Generated code.

#include <model.pb.h>
#include <string>
#include <vector>

''')

# Define enums.
for enum in enums:
    args.hpp_file.write('\nenum class {} {{\n'.format(enum[0]))
    for enumerator in enum[1]:
        args.hpp_file.write('    {} = {},\n'.format(enumerator[0], enumerator[1]))
    args.hpp_file.write('};\n')

# Define structs.
for message in messages:
    args.hpp_file.write('\nstruct {}\n{{\n'.format(message[0]))
    for member in message[1]:
        args.hpp_file.write('    ')
        if member[0]: # repeated
            args.hpp_file.write('std::vector<{}> {};\n'.format(get_cpp_type(member[1]), member[2]))
        elif type(member[1]) is tuple: # map
            args.hpp_file.write('std::map<{}, {}> {};\n'.format(get_cpp_type(member[1][0]), get_cpp_type(member[1][1]), member[2]))
        else: # singular value
            args.hpp_file.write('{} {};\n'.format(get_cpp_type(member[1]), member[2]))
    args.hpp_file.write('};\n')

# Declare {To,From}Protobuf() functions.
for message in messages:
    args.hpp_file.write('''
message::{0} ToProtobuf (const {0}& value);
{0} FromProtobuf (const message::{0}& msg);
'''.format(message[0]))


args.cpp_file.write('''
// WARNING: Generated code.

#include "{}"

'''.format(os.path.basename(args.hpp_file.name)))

# Define ToProtobuf() functions.
for message in messages:
    args.cpp_file.write('''
message::{0} ToProtobuf (const {0}& value)
{{
    message::{0} retval;
'''.format(message[0]))

    for member in message[1]:
        name = member[2]
        value_type = get_value_type(member[1])
        message_type = value_type in message_names
        enum_type = value_type in enum_names
        if member[0]: # repeated
            if message_type:
                loop_stmt = 'retval.add_{name}()->CopyFrom(ToProtobuf(x));'.format(**locals())
            elif enum_type:
                loop_stmt = 'retval.add_{name}(static_cast<message::{value_type}>(x));'.format(**locals())
            else:
                loop_stmt = 'retval.add_{name}(x);'.format(**locals())
            args.cpp_file.write('''    for (const auto& x : value.{name}) {{
        {loop_stmt}
    }}
'''.format(**locals()))
        elif type(member[1]) is tuple: # map
            if message_type:
                loop_stmt = '(*retval.mutable_{name}())[x.first] = ToProtobuf(x.second);'.format(**locals())
            elif enum_type:
                loop_stmt = '(*retval.mutable_{name}())[x.first] = static_cast<message::{value_type}>(x.second);'.format(**locals())
            else:
                loop_stmt = '(*retval.mutable_{name}())[x.first] = x.second;'.format(**locals())
            args.cpp_file.write('''    for (const auto& x : value.{name}) {{
        {loop_stmt}
    }}
'''.format(**locals()))
        else: # singular value
            if message_type:
                args.cpp_file.write('    retval.mutable_{name}()->CopyFrom(ToProtobuf(value.{name}));\n'.format(**locals()))
            elif enum_type:
                args.cpp_file.write('    retval.set_{name}(static_cast<message::{value_type}>(value.{name}));\n'.format(**locals()))
            else:
                args.cpp_file.write('    retval.set_{name}(value.{name});\n'.format(**locals()))

    args.cpp_file.write('''    return retval;
}
''');

# Define FromProtobuf() functions.
for message in messages:
    args.cpp_file.write('''
{0} FromProtobuf (const message::{0}& msg)
{{
    {0} retval;
'''.format(message[0]))

    for member in message[1]:
        name = member[2]
        value_type = get_value_type(member[1])
        message_type = value_type in message_names
        enum_type = value_type in enum_names
        if member[0]: # repeated
            if message_type:
                loop_stmt = '*it++ = FromProtobuf(x);'.format(**locals())
            elif enum_type:
                loop_stmt = '*it++ = static_cast<{value_type}>(x);'.format(**locals())
            else:
                loop_stmt = '*it++ = x;'.format(**locals())
            args.cpp_file.write('''    retval.{name}.resize(msg.{name}_size());
    auto it = retval.{name}.begin();
    for (const auto& x : msg.{name}()) {{
        {loop_stmt}
    }}
'''.format(**locals()))
        elif type(member[1]) is tuple: # map
            if message_type:
                loop_stmt = 'retval.{name}[x.first] = FromProtobuf(x.second);'.format(**locals())
            elif enum_type:
                loop_stmt = 'retval.{name}[x.first] = static_cast<{value_type}>(x.second);'.format(**locals())
            else:
                loop_stmt = 'retval.{name}[x.first] = x.second;'.format(**locals())
            args.cpp_file.write('''    for (const auto& x : msg.{name}()) {{
        {loop_stmt}
    }}
'''.format(**locals()))
        else: # singular value
            if message_type:
                args.cpp_file.write('    retval.{name} = FromProtobuf(msg.{name}());\n'.format(**locals()))
            elif enum_type:
                args.cpp_file.write('    retval.{name} = static_cast<{value_type}>(msg.{name}());\n'.format(**locals()))
            else:
                args.cpp_file.write('    retval.{name} = msg.{name}();\n'.format(**locals()))

    args.cpp_file.write('''    return retval;
}
''');
