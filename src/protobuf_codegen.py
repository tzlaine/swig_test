#!/usr/bin/env python

import argparse

parser = argparse.ArgumentParser(description='Generate C++ structs and binding layers from Protobuf messages.')
parser.add_argument('--site-packages', required=True, type=str,
                    help='Path to the built Protobuf Python files (usually called "site-packages"')
parser.add_argument('--proto-file', required=True, type=argparse.FileType('r'),
                    help='Original .proto file')
parser.add_argument('--desc-file', required=True, type=argparse.FileType('rb'),
                    help='Input proto description file (binary)')
parser.add_argument('--namespace', type=str,
                    help='Namespace into which to place generated code (e.g. foo::bar).')
parser.add_argument('--cpp-file', required=True, type=argparse.FileType('w'),
                    help='Output .cpp file')
parser.add_argument('--hpp-file', required=True, type=argparse.FileType('w'),
                    help='Output .hpp file')
parser.add_argument('--cs-file', required=True, type=argparse.FileType('w'),
                    help='Output .cs file')
args = parser.parse_args()

import site
site.addsitedir(args.site_packages)

from google.protobuf import descriptor_pb2
import re
import os

proto_contents = args.proto_file.read()

# Tuples of (c++-type, c#-type).  Using 'void' for several of these
# that are not yet supported, to cause compilation failures in the
# generated types.  Comments below from descriptor.proto in the
# Protobuf sources.
types = [
    ('void', 'void'),          #    // 0 is reserved for errors.
    ('double', 'double'),      #    TYPE_DOUBLE         = 1;
    ('float', 'float'),        #    TYPE_FLOAT          = 2;
    ('void', 'void'),          #    TYPE_INT64          = 3;
    ('void', 'void'),          #    TYPE_UINT64         = 4;
    ('int', 'int'),            #    TYPE_INT32          = 5;
    ('void', 'void'),          #    TYPE_FIXED64        = 6;
    ('int', 'int'),            #    TYPE_FIXED32        = 7;
    ('bool', 'bool'),          #    TYPE_BOOL           = 8;
    ('std::string', 'string'), #    TYPE_STRING         = 9;
    ('void', 'void'),          #    TYPE_GROUP          = 10;  // Tag-delimited aggregate.
    ('void', 'void'),          #    TYPE_MESSAGE        = 11;  // Length-delimited aggregate.
    ('void', 'void'),          #    TYPE_BYTES          = 12;
    ('unsigned int', 'uint'),  #    TYPE_UINT32         = 13;
    ('void', 'void'),          #    TYPE_ENUM           = 14;
    ('int', 'int'),            #    TYPE_SFIXED32       = 15;
    ('void', 'void'),          #    TYPE_SFIXED64       = 16;
    ('int', 'int'),            #    TYPE_SINT32         = 17;  // Uses ZigZag encoding.
    ('void', 'void')           #    TYPE_SINT64         = 18;  // Uses ZigZag encoding.
]

def get_cpp_type(typecode):
    retval = 'void'
    if typecode < len(types):
        retval = types[typecode][0]
    return retval

def get_csharp_type(typecode):
    retval = 'void'
    if typecode < len(types):
        retval = types[typecode][1]
    return retval

def map_field_entry_type(descriptor_proto):
    return descriptor_proto.options.map_entry == True

def type_without_namespace(field_descriptor_proto, namespace):
    typenames = field_descriptor_proto.type_name.split('.')[1:]
    prefix = filter(lambda x: x[0] == x[1], zip(typenames, namespace))
    return '.'.join(typenames[len(prefix):])

def to_cpp_namespace(typename):
    return typename.replace('.', '::')

def indent_str(i):
    return '    ' * i

def indent(i):
    args.cpp_file.write(indent_str(i))
    args.hpp_file.write(indent_str(i))

def newline():
    args.cpp_file.write('\n')
    args.hpp_file.write('\n')

def add_header_comment_and_includes(proto_source, syntax):
    args.cpp_file.write('''// WARNING: Generated code.
// This file was generated from {} ({})

#include "{}"


'''.format(proto_source, syntax, os.path.basename(args.hpp_file.name)))

    args.hpp_file.write('''// WARNING: Generated code.
// This file was generated from {0} ({1})
#pragma once

#include <{0}.pb.h>
#include <string>
#include <vector>
#include <boost/container/flat_map.hpp>


'''.format(os.path.splitext(proto_source)[0], syntax))

def open_namespace(namespace, depth=0):
    indent(depth)
    for name in namespace:
        args.cpp_file.write('namespace {} {{ '.format(name))
        args.hpp_file.write('namespace {} {{ '.format(name))
    newline()
    return depth + (len(namespace) and 1 or 0)

def close_namespace(namespace, depth=0):
    indent(depth)
    newline()
    for name in namespace:
        args.cpp_file.write('} ')
        args.hpp_file.write('} ')
    newline()

def handle_enum_descriptor_proto(enum_descriptor_proto, depth):
    args.hpp_file.write('\n')
    args.hpp_file.write('{}enum class {} {{\n'.format(indent_str(depth), enum_descriptor_proto.name))
    depth += 1
    for enum_value_descriptor_proto in enum_descriptor_proto.value:
        args.hpp_file.write('{}{} = {},\n'.format(indent_str(depth), enum_value_descriptor_proto.name, enum_value_descriptor_proto.number))
    depth -= 1
    args.hpp_file.write('{}}};\n'.format(indent_str(depth)))

    if args.cs_file:
        args.cs_file.write('\n')
        args.cs_file.write('{}public enum {} {{\n'.format(indent_str(depth), enum_descriptor_proto.name))
        depth += 1
        for enum_value_descriptor_proto in enum_descriptor_proto.value:
            args.cs_file.write('{}{} = {},\n'.format(indent_str(depth), enum_value_descriptor_proto.name, enum_value_descriptor_proto.number))
        depth -= 1
        args.cs_file.write('{}}};\n'.format(indent_str(depth)))

def repeated(field_descriptor_proto):
    return field_descriptor_proto.label is field_descriptor_proto.LABEL_REPEATED

def field_type(field_descriptor_proto, lang):
    if field_descriptor_proto.type is field_descriptor_proto.TYPE_MESSAGE or \
       field_descriptor_proto.type is field_descriptor_proto.TYPE_ENUM:
        typename = field_descriptor_proto.type_name.split('.')[-1]
    else:
        if lang is 'cpp':
            typename = get_cpp_type(field_descriptor_proto.type)
        else:
            typename = get_csharp_type(field_descriptor_proto.type)
    if repeated(field_descriptor_proto):
        if lang is 'cpp':
            typename = 'std::vector<{}>'.format(typename)
        else:
            typename = '{}[]'.format(typename)
    return typename

def declare_field_descriptor_proto(field_descriptor_proto, depth, map_fields):
    leaf_type = type_without_namespace(field_descriptor_proto, protobuf_namespace)
    if leaf_type in map_fields:
        key_type = field_type(map_fields[leaf_type].field[0], 'cpp')
        value_type = field_type(map_fields[leaf_type].field[1], 'cpp')
        args.hpp_file.write('{}boost::container::flat_map<{}, {}> {};\n'.format(indent_str(depth), key_type, value_type, field_descriptor_proto.name))
    else:
        typename = field_type(field_descriptor_proto, 'cpp')
        args.hpp_file.write('{}{} {};\n'.format(indent_str(depth), typename, field_descriptor_proto.name))
    if args.cs_file:
        if leaf_type in map_fields:
            key_type = field_type(map_fields[leaf_type].field[0], 'csharp')
            value_type = field_type(map_fields[leaf_type].field[1], 'csharp')
            args.cs_file.write('{}public Dictionary<{}, {}> {};\n'.format(indent_str(depth), key_type, value_type, field_descriptor_proto.name))
        else:
            typename = field_type(field_descriptor_proto, 'csharp')
            args.cs_file.write('{}public {} {};\n'.format(indent_str(depth), typename, field_descriptor_proto.name))

def declare_descriptor_proto(descriptor_proto, protobuf_namespace, user_namespace, depth, scope, to_decls, from_decls, map_fields):
    args.hpp_file.write('\n')
    args.hpp_file.write('{0}struct {1}\n{0}{{\n'.format(indent_str(depth), descriptor_proto.name))
    if args.cs_file:
        args.cs_file.write('\n')
        args.cs_file.write('{0}public class {1}\n{0}{{\n'.format(indent_str(depth), descriptor_proto.name))
    depth += 1

    for enum_descriptor_proto in descriptor_proto.enum_type:
        handle_enum_descriptor_proto(enum_descriptor_proto, depth)

    for descriptor in descriptor_proto.nested_type:
        if map_field_entry_type(descriptor):
            field = None
            for f in descriptor_proto.field:
                if f.type_name.endswith(descriptor.name):
                    field = f
                    break
            name = type_without_namespace(field, protobuf_namespace)
            map_fields[name] = descriptor
        else:
            declare_descriptor_proto(descriptor, protobuf_namespace, user_namespace, depth, scope + [descriptor_proto.name], to_decls, from_decls, map_fields)

    for field_descriptor_proto in descriptor_proto.field:
        declare_field_descriptor_proto(field_descriptor_proto, depth, map_fields)
    depth -= 1
    args.hpp_file.write('{}}};\n'.format(indent_str(depth)))
    if args.cs_file:
        args.cs_file.write('{}}};\n'.format(indent_str(depth)))

    scope_ns = '::'.join(scope)
    proto_ns = '::'.join(protobuf_namespace)
    user_ns = '::'.join(user_namespace)
    proto_ns_with_scope = proto_ns
    user_ns_with_scope = user_ns
    if scope_ns != '':
        proto_ns_with_scope += '::' + scope_ns
        user_ns_with_scope += '::' + scope_ns
    to_decl = (
        '{0}::{1}'.format(proto_ns_with_scope, descriptor_proto.name),
        'to_protobuf (const {0}::{1}& value)'.format(user_ns_with_scope, descriptor_proto.name),
        descriptor_proto
    )
    from_decl = (
        '{0}::{1}'.format(user_ns_with_scope, descriptor_proto.name),
        'from_protobuf (const {0}::{1}& msg)'.format(proto_ns_with_scope, descriptor_proto.name),
        descriptor_proto,
        proto_ns,
        scope,
        descriptor_proto.name
    )
    to_decls.append(to_decl)
    from_decls.append(from_decl)

def define_to_impl_field(field_descriptor_proto, depth, map_fields):
    leaf_type = type_without_namespace(field_descriptor_proto, protobuf_namespace)
    proto_ns = '::'.join(protobuf_namespace)
    if repeated(field_descriptor_proto):
        args.cpp_file.write('{0}for (const auto& x : value.{1}) {{\n'.format(indent_str(depth), field_descriptor_proto.name))
        depth += 1
        if leaf_type in map_fields:
            value_field = map_fields[leaf_type].field[1]
            if value_field.type is value_field.TYPE_MESSAGE:
                args.cpp_file.write('{0}(*retval.mutable_{1}())[x.first] = to_protobuf(x.second);\n'.format(indent_str(depth), field_descriptor_proto.name))
            elif value_field.type is value_field.TYPE_ENUM:
                value_type = field_type(value_field, 'cpp')
                args.cpp_file.write('{0}(*retval.mutable_{1}())[x.first] = static_cast< {2}::{3} >(x.second);\n'.format(indent_str(depth), field_descriptor_proto.name, proto_ns, to_cpp_namespace(leaf_type)))
            else:
                args.cpp_file.write('{0}(*retval.mutable_{1}())[x.first] = x.second;\n'.format(indent_str(depth), field_descriptor_proto.name))
        elif field_descriptor_proto.type is field_descriptor_proto.TYPE_MESSAGE:
            args.cpp_file.write('{0}retval.add_{1}()->CopyFrom(to_protobuf(x));\n'.format(indent_str(depth), field_descriptor_proto.name))
        elif field_descriptor_proto.type is field_descriptor_proto.TYPE_ENUM:
            args.cpp_file.write('{0}retval.add_{1}(static_cast< {2}::{3} >(x));\n'.format(indent_str(depth), field_descriptor_proto.name, proto_ns, to_cpp_namespace(leaf_type)))
        else:
            args.cpp_file.write('{0}retval.add_{1}(x);\n'.format(indent_str(depth), field_descriptor_proto.name))
        depth -= 1
        args.cpp_file.write('{0}}}\n'.format(indent_str(depth)))
    else:
        if field_descriptor_proto.type is field_descriptor_proto.TYPE_MESSAGE:
            args.cpp_file.write('{0}retval.mutable_{1}()->CopyFrom(to_protobuf(value.{1}));\n'.format(indent_str(depth), field_descriptor_proto.name))
        elif field_descriptor_proto.type is field_descriptor_proto.TYPE_ENUM:
            args.cpp_file.write('{0}retval.set_{1}(static_cast< {2}::{3} >(value.{1}));\n'.format(indent_str(depth), field_descriptor_proto.name, proto_ns, to_cpp_namespace(leaf_type)))
        else:
            args.cpp_file.write('{0}retval.set_{1}(value.{1});\n'.format(indent_str(depth), field_descriptor_proto.name))

def define_to_impl(to_decl, depth, map_fields):
    args.cpp_file.write('''
{0}{1} {2}
{0}{{
'''.format(indent_str(depth), to_decl[0], to_decl[1]))
    descriptor_proto = to_decl[2]
    depth += 1
    args.cpp_file.write('{0}{1} retval;\n'.format(indent_str(depth), to_decl[0]))
    for field_descriptor_proto in descriptor_proto.field:
        define_to_impl_field(field_descriptor_proto, depth, map_fields)
    args.cpp_file.write('{0}return retval;\n'.format(indent_str(depth)))
    depth -= 1
    args.cpp_file.write('{0}}}\n'.format(indent_str(depth)))

def define_from_impl_field(field_descriptor_proto, depth, map_fields):
    leaf_type = type_without_namespace(field_descriptor_proto, protobuf_namespace)
    proto_ns = '::'.join(protobuf_namespace)
    if repeated(field_descriptor_proto):
        args.cpp_file.write('{0}{{\n'.format(indent_str(depth)))
        depth += 1
        if leaf_type not in map_fields:
            args.cpp_file.write('''{0}retval.{1}.resize(msg.{1}_size());
{0}auto it = retval.{1}.begin();
'''.format(indent_str(depth), field_descriptor_proto.name))
        args.cpp_file.write('{0}for (const auto& x : msg.{1}()) {{\n'.format(indent_str(depth), field_descriptor_proto.name))
        depth += 1
        if leaf_type in map_fields:
            value_field = map_fields[leaf_type].field[1]
            if value_field.type is value_field.TYPE_MESSAGE:
                args.cpp_file.write('{0}retval.{1}[x.first] = from_protobuf(x.second);\n'.format(indent_str(depth), field_descriptor_proto.name))
            elif value_field.type is value_field.TYPE_ENUM:
                value_type = field_type(value_field, 'cpp')
                args.cpp_file.write('{0}retval.{1}[x.first] = static_cast<{2}>(x.second);\n'.format(indent_str(depth), field_descriptor_proto.name, to_cpp_namespace(leaf_type)))
            else:
                args.cpp_file.write('{0}retval.{1}[x.first] = x.second;\n'.format(indent_str(depth), field_descriptor_proto.name))
        elif field_descriptor_proto.type is field_descriptor_proto.TYPE_MESSAGE:
            args.cpp_file.write('{0}*it++ = from_protobuf(x);\n'.format(indent_str(depth)))
        elif field_descriptor_proto.type is field_descriptor_proto.TYPE_ENUM:
            value_type = field_type(field_descriptor_proto, 'cpp')
            args.cpp_file.write('{0}*it++ = static_cast<{1}>(x);\n'.format(indent_str(depth), to_cpp_namespace(leaf_type)))
        else:
            args.cpp_file.write('{0}*it++ = x;\n'.format(indent_str(depth), field_descriptor_proto.name))
        depth -= 1
        args.cpp_file.write('{0}}}\n'.format(indent_str(depth)))
        depth -= 1
        args.cpp_file.write('{0}}}\n'.format(indent_str(depth)))
    else:
        if field_descriptor_proto.type is field_descriptor_proto.TYPE_MESSAGE:
            args.cpp_file.write('{0}retval.{1} = from_protobuf(msg.{1}());\n'.format(indent_str(depth), field_descriptor_proto.name))
        elif field_descriptor_proto.type is field_descriptor_proto.TYPE_ENUM:
            args.cpp_file.write('{0}retval.{1} = static_cast< {2} >(msg.{1}());\n'.format(indent_str(depth), field_descriptor_proto.name, to_cpp_namespace(leaf_type)))
        else:
            args.cpp_file.write('{0}retval.{1} = msg.{1}();\n'.format(indent_str(depth), field_descriptor_proto.name))

def define_from_impl(from_decl, depth, map_fields):
    args.cpp_file.write('''
{0}{1} {2}
{0}{{
'''.format(indent_str(depth), from_decl[0], from_decl[1]))
    descriptor_proto = from_decl[2]
    depth += 1
    args.cpp_file.write('{0}{1} retval;\n'.format(indent_str(depth), from_decl[0]))
    for field_descriptor_proto in descriptor_proto.field:
        define_from_impl_field(field_descriptor_proto, depth, map_fields)
    args.cpp_file.write('{0}return retval;\n'.format(indent_str(depth)))
    depth -= 1
    args.cpp_file.write('{0}}}\n'.format(indent_str(depth)))

def to_csharp_namespace(namespace):
    if type(namespace) is list:
        temp_list = namespace
    else:
        temp_list = namespace.split('::')
    def pascal_case(s):
        s_pieces = s.split('_')
        return ''.join(map(lambda x: x.capitalize(), s_pieces))
    return '.'.join(map(lambda x: pascal_case(x), temp_list))

def define_csharp_from(from_decl, map_fields):
    ns = to_csharp_namespace(from_decl[3])
    typename = from_decl[5]
    if len(from_decl[4]):
        typename = '.Types.'.join(from_decl[4]) + '.Types.' + typename
    if len(ns):
        typename = ns + '.' + typename
    args.cs_file.write('''public static SimpleJSON.JSONNode from_protobuf({} msg)
{{
'''.format(typename))
    args.cs_file.write('    return null; // TODO\n')
    args.cs_file.write('}\n\n')

def define_csharp_to(from_decl, map_fields):
    ns = to_csharp_namespace(from_decl[3])
    typename = from_decl[5]
    if len(from_decl[4]):
        typename = '.Types.'.join(from_decl[4]) + '.Types.' + typename
    fn_name = 'to_' + typename.replace('.', '_')
    if len(ns):
        typename = ns + '.' + typename
    args.cs_file.write('''public static {} {}(SimpleJSON.JSONNode json)
{{
'''.format(typename, fn_name))
    args.cs_file.write('    return null; // TODO\n')
    args.cs_file.write('}\n\n')

file_descriptor_set = descriptor_pb2.FileDescriptorSet()
file_descriptor_set.ParseFromString(args.desc_file.read())
for field_descriptor_proto in file_descriptor_set.file:
    proto_source = field_descriptor_proto.name
    syntax = str(field_descriptor_proto.syntax)
    protobuf_namespace = str(field_descriptor_proto.package).split('.')

    add_header_comment_and_includes(proto_source, syntax)

    if args.cs_file:
        args.cs_file.write('''// WARNING: Generated code.
// This file was generated from {} ({})

using System.Collections.Generic;

namespace model_data {{
'''.format(proto_source, syntax))

    user_namespace = args.namespace and args.namespace.split('::') or []
    depth = open_namespace(user_namespace)

    for enum_descriptor_proto in field_descriptor_proto.enum_type:
        handle_enum_descriptor_proto(enum_descriptor_proto, depth)

    to_decls = []
    from_decls = []
    map_fields = {}
    for descriptor_proto in field_descriptor_proto.message_type:
        declare_descriptor_proto(descriptor_proto, protobuf_namespace, user_namespace, depth, [], to_decls, from_decls, map_fields)

    if args.cs_file:
        args.cs_file.write('''
public class convert
{{

'''.format(proto_source, syntax, to_csharp_namespace(protobuf_namespace)))

    for i in range(len(to_decls)):
        args.hpp_file.write('''
{0}{1} {2};
{0}{3} {4};
'''.format(indent_str(depth), to_decls[i][0], to_decls[i][1], from_decls[i][0], from_decls[i][1]))
        define_to_impl(to_decls[i], depth, map_fields)
        define_from_impl(from_decls[i], depth, map_fields)

        if False and args.cs_file:
            define_csharp_from(from_decls[i], map_fields)
            define_csharp_to(from_decls[i], map_fields)

    close_namespace(user_namespace)
    
    if args.cs_file:
        args.cs_file.write('};\n\n}\n')
