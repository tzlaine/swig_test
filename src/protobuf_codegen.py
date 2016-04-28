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
parser.add_argument('--cs-file', type=argparse.FileType('w'),
                    help='Output .cs file')
parser.add_argument('--cs-namespace', type=str,
                    help='C# namespace into which to place generated code (e.g. foo.bar).')
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

def add_header_comment_and_includes(proto_source, syntax, deps):
    cpp_deps = '\n'.join(map(lambda x: '#include <{}.hpp>'.format(x), deps))

    args.cpp_file.write('''// WARNING: Generated code.
// This file was generated from {} ({})

#ifndef BUILD_FOR_TEST
#include "Spaceport.h"
#endif

#include "{}"
{}


'''.format(proto_source, syntax, os.path.basename(args.hpp_file.name), cpp_deps))

    args.hpp_file.write('''// WARNING: Generated code.
// This file was generated from {0} ({1})
#pragma once

#include <{0}.pb.h>
{2}

#include <string>
#include <vector>
#include <boost/container/flat_map.hpp>


'''.format(os.path.splitext(proto_source)[0], syntax, cpp_deps))

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

def field_element_type(field_descriptor_proto, lang):
    if field_descriptor_proto.type is field_descriptor_proto.TYPE_MESSAGE or \
       field_descriptor_proto.type is field_descriptor_proto.TYPE_ENUM:
        typename = type_without_namespace(field_descriptor_proto, protobuf_namespace)
        if lang is 'cpp':
            typename = typename.replace('.', '::')
    else:
        if lang is 'cpp':
            typename = get_cpp_type(field_descriptor_proto.type)
        else:
            typename = get_csharp_type(field_descriptor_proto.type)
    return typename

def field_type(field_descriptor_proto, lang):
    typename = field_element_type(field_descriptor_proto, lang)
    if repeated(field_descriptor_proto):
        if lang is 'cpp':
            typename = 'std::vector<{}>'.format(typename)
        else:
            typename = '{}[]'.format(typename)
    return typename

def to_csharp_namespace(namespace):
    def pascal_case(s):
        s_pieces = s.split('_')
        return ''.join(map(lambda x: x.capitalize(), s_pieces))
    return '.'.join(map(lambda x: pascal_case(x), namespace))

def make_decl_impl(decl_data, kind, return_type_only):
    kind_args = kind.split('+')
    if kind_args[2] == 'cpp':
        scope_ns = '::'.join(decl_data['scope_ns'])
        proto_ns = '::'.join(decl_data['proto_ns'])
        user_ns = '::'.join(decl_data['user_ns'])
        proto_ns_with_scope = proto_ns
        user_ns_with_scope = user_ns
        if scope_ns != '':
            proto_ns_with_scope += '::' + scope_ns
            user_ns_with_scope += '::' + scope_ns
        if kind_args[1] == 'pb':
            if kind_args[0] == 'to':
                format_str = return_type_only and '{1}::{2}' or '{1}::{2} to_protobuf (const {0}::{2}& value)'
                return format_str.format(user_ns_with_scope, proto_ns_with_scope, decl_data['name'])
            else:
                format_str = return_type_only and '{0}::{2}' or '{0}::{2} from_protobuf (const {1}::{2}& msg)'
                return format_str.format(user_ns_with_scope, proto_ns_with_scope, decl_data['name'])
        elif kind_args[1] == 'bin':
            if kind_args[0] == 'to':
                format_str = return_type_only and 'void' or 'void to_bin (const {0}::{1}& value, std::vector<unsigned char>& bin)'
                return format_str.format(user_ns_with_scope, decl_data['name'])
            else:
                typename = decl_data['name']
                if len(decl_data['scope_ns']):
                    typename = '_'.join(decl_data['scope_ns']) + '_' + typename
                format_str = return_type_only and '{0}::{1}' or '{0}::{1} {2}_from_bin (unsigned char*& bin)'
                return format_str.format(user_ns_with_scope, decl_data['name'], typename)
        else:
            raise Exception('{to,from}+<non-pb>+cpp not yet implemented')
    elif kind_args[2] == 'cs':
        typename = decl_data['name']
        if len(decl_data['scope_ns']):
            typename = '.'.join(decl_data['scope_ns']) + '.' + typename
        fn_name = 'to_' + typename.replace('.', '_')

        if kind_args[1] == 'pb':
            raise Exception('{to,from}+pb+cs not defined')
        elif kind_args[1] == 'bin':
            if kind_args[0] == 'to':
                format_str = return_type_only and 'void' or 'void to_bin({} value, byte[] bin, ref int offset)'
                return format_str.format(typename)
            else:
                format_str = return_type_only and '{0}' or '{0} {1}_from_bin(byte[] bin, ref int offset)'
                return format_str.format(typename, typename.replace('.', '_'))
        elif kind_args[1] == 'size':
            return 'int size({} value)'.format(typename)
        else:
            raise Exception('{to,from}+<non-bin>+cpp not yet implemented')
    else:
        raise Exception('Unknown language {} in {}'.format(kind_args[2], kind))

def make_decl(decl_data, kind):
    return make_decl_impl(decl_data, kind, False)

def make_return_type(decl_data, kind):
    return make_decl_impl(decl_data, kind, True)

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

def declare_descriptor_proto(descriptor_proto, protobuf_namespace, user_namespace, depth, scope, all_decl_data, map_fields):
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
            declare_descriptor_proto(descriptor, protobuf_namespace, user_namespace, depth, scope + [descriptor_proto.name], all_decl_data, map_fields)

    for field_descriptor_proto in descriptor_proto.field:
        declare_field_descriptor_proto(field_descriptor_proto, depth, map_fields)
    depth -= 1
    args.hpp_file.write('{}}};\n'.format(indent_str(depth)))
    if args.cs_file:
        args.cs_file.write('{}}};\n'.format(indent_str(depth)))

    decl_data = {
        'scope_ns': scope,
        'proto_ns': protobuf_namespace,
        'user_ns': user_namespace,
        'name': descriptor_proto.name,
        'descriptor_proto': descriptor_proto
    }
    all_decl_data.append(decl_data)

def define_cpp_to_pb_impl_field(field_descriptor_proto, depth, map_fields):
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

def define_cpp_to_pb_impl(decl_data, depth, map_fields):
    args.cpp_file.write('''
{0}{1}
{0}{{
'''.format(indent_str(depth), make_decl(decl_data, 'to+pb+cpp')))
    descriptor_proto = decl_data['descriptor_proto']
    depth += 1
    args.cpp_file.write('{0}{1} retval;\n'.format(indent_str(depth), make_return_type(decl_data, 'to+pb+cpp')))
    for field_descriptor_proto in descriptor_proto.field:
        define_cpp_to_pb_impl_field(field_descriptor_proto, depth, map_fields)
    args.cpp_file.write('{0}return retval;\n'.format(indent_str(depth)))
    depth -= 1
    args.cpp_file.write('{0}}}\n'.format(indent_str(depth)))

def define_cpp_from_pb_impl_field(field_descriptor_proto, depth, map_fields):
    leaf_type = type_without_namespace(field_descriptor_proto, protobuf_namespace)
    proto_ns = '::'.join(protobuf_namespace)
    user_ns = '::'.join(user_namespace)
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
                args.cpp_file.write('{0}retval.{1}[x.first] = static_cast<std::remove_reference<decltype(retval.{1}[x.first])>::type>(x.second);\n'.format(indent_str(depth), field_descriptor_proto.name))
            else:
                args.cpp_file.write('{0}retval.{1}[x.first] = x.second;\n'.format(indent_str(depth), field_descriptor_proto.name))
        elif field_descriptor_proto.type is field_descriptor_proto.TYPE_MESSAGE:
            args.cpp_file.write('{0}*it++ = from_protobuf(x);\n'.format(indent_str(depth)))
        elif field_descriptor_proto.type is field_descriptor_proto.TYPE_ENUM:
            value_type = field_type(field_descriptor_proto, 'cpp')
            args.cpp_file.write('{0}*it++ = static_cast<std::remove_reference<decltype(*it++)>::type>(x);\n'.format(indent_str(depth)))
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
            args.cpp_file.write('{0}retval.{1} = static_cast<std::remove_reference<decltype(retval.{1})>::type>(msg.{1}());\n'.format(indent_str(depth), field_descriptor_proto.name))
        else:
            args.cpp_file.write('{0}retval.{1} = msg.{1}();\n'.format(indent_str(depth), field_descriptor_proto.name))

def define_cpp_from_pb_impl(decl_data, depth, map_fields):
    args.cpp_file.write('''
{0}{1}
{0}{{
'''.format(indent_str(depth), make_decl(decl_data, 'from+pb+cpp')))
    descriptor_proto = decl_data['descriptor_proto']
    depth += 1
    args.cpp_file.write('{0}{1} retval;\n'.format(indent_str(depth), make_return_type(decl_data, 'from+pb+cpp')))
    for field_descriptor_proto in descriptor_proto.field:
        define_cpp_from_pb_impl_field(field_descriptor_proto, depth, map_fields)
    args.cpp_file.write('{0}return retval;\n'.format(indent_str(depth)))
    depth -= 1
    args.cpp_file.write('{0}}}\n'.format(indent_str(depth)))

def define_cpp_to_bin_impl_field(field_descriptor_proto, depth, map_fields):
    leaf_type = type_without_namespace(field_descriptor_proto, protobuf_namespace)
    proto_ns = '::'.join(protobuf_namespace)
    if repeated(field_descriptor_proto):
        args.cpp_file.write('{0}{{\n'.format(indent_str(depth)))
        depth += 1
        args.cpp_file.write('''{0}int length = static_cast<int>(value.{1}.size());
{0}to_bin(length, bin);
{0}for (const auto& x : value.{1}) {{
'''.format(indent_str(depth), field_descriptor_proto.name))
        depth += 1
        if leaf_type in map_fields:
            value_field = map_fields[leaf_type].field[1]
            args.cpp_file.write('''{0}to_bin(x.first, bin);
{0}to_bin(x.second, bin);
'''.format(indent_str(depth)))
        else:
            args.cpp_file.write('{0}to_bin(x, bin);\n'.format(indent_str(depth)))
        depth -= 1
        args.cpp_file.write('{0}}}\n'.format(indent_str(depth)))
        depth -= 1
        args.cpp_file.write('{0}}}\n'.format(indent_str(depth)))
    else:
        args.cpp_file.write('{0}to_bin(value.{1}, bin);\n'.format(indent_str(depth), field_descriptor_proto.name))

def define_cpp_to_bin_impl(decl_data, depth, map_fields):
    args.cpp_file.write('''
{0}{1}
{0}{{
'''.format(indent_str(depth), make_decl(decl_data, 'to+bin+cpp')))
    descriptor_proto = decl_data['descriptor_proto']
    depth += 1
    for field_descriptor_proto in descriptor_proto.field:
        define_cpp_to_bin_impl_field(field_descriptor_proto, depth, map_fields)
    depth -= 1
    args.cpp_file.write('{0}}}\n'.format(indent_str(depth)))

def define_cpp_from_bin_impl_field(field_descriptor_proto, depth, map_fields):
    leaf_type = type_without_namespace(field_descriptor_proto, protobuf_namespace)
    field_type = field_element_type(field_descriptor_proto, 'cpp')
    if repeated(field_descriptor_proto):
        args.cpp_file.write('{0}{{\n'.format(indent_str(depth)))
        depth += 1
        args.cpp_file.write('{0}int length = int_from_bin(bin);\n'.format(indent_str(depth)))
        if leaf_type not in map_fields:
            args.cpp_file.write('{0}retval.{1}.resize(length);\n'.format(indent_str(depth), field_descriptor_proto.name))
        args.cpp_file.write('{0}for (int i = 0; i < length; ++i) {{\n'.format(indent_str(depth), field_descriptor_proto.name))
        depth += 1
        if leaf_type in map_fields:
            key_field = map_fields[leaf_type].field[0]
            key_field_type = field_element_type(key_field, 'cpp')
            if key_field.type is key_field.TYPE_STRING:
                args.cpp_file.write('{0}auto key = string_from_bin(bin);\n'.format(indent_str(depth)))
            elif key_field.type is key_field.TYPE_ENUM:
                args.cpp_file.write('{0}auto key = enum_from_bin< {1} >(bin);\n'.format(indent_str(depth), type_without_namespace(key_field, protobuf_namespace)))
            else:
                args.cpp_file.write('{0}auto key = {1}_from_bin(bin);\n'.format(indent_str(depth), key_field_type))
            value_field = map_fields[leaf_type].field[1]
            value_field_type = field_element_type(value_field, 'cpp')
            if value_field.type is value_field.TYPE_MESSAGE:
                args.cpp_file.write('{0}auto value = {2}_from_bin(bin);\n'.format(indent_str(depth), value_field_type, value_field_type.replace('::', '_')))
            elif value_field.type is value_field.TYPE_STRING:
                args.cpp_file.write('{0}auto value = string_from_bin(bin);\n'.format(indent_str(depth)))
            elif value_field.type is value_field.TYPE_ENUM:
                args.cpp_file.write('{0}auto value = enum_from_bin< {1} >(bin);\n'.format(indent_str(depth), to_cpp_namespace(type_without_namespace(value_field, protobuf_namespace))))
            else:
                args.cpp_file.write('{0}auto value = {1}_from_bin(bin);\n'.format(indent_str(depth), value_field_type))
            args.cpp_file.write('{0}retval.{1}[key] = value;\n'.format(indent_str(depth), field_descriptor_proto.name))
        else:
            if field_descriptor_proto.type is field_descriptor_proto.TYPE_STRING:
                args.cpp_file.write('{0}auto x = string_from_bin(bin);\n'.format(indent_str(depth)))
            elif field_descriptor_proto.type is field_descriptor_proto.TYPE_ENUM:
                args.cpp_file.write('{0}auto x = enum_from_bin<std::remove_reference<decltype(retval.{1}[i])>::type>(bin);\n'.format(indent_str(depth), field_descriptor_proto.name))
            else:
                args.cpp_file.write('{0}auto x = {2}_from_bin(bin);\n'.format(indent_str(depth), field_type, field_type.replace('::', '_')))
            args.cpp_file.write('{0}retval.{1}[i] = x;\n'.format(indent_str(depth), field_descriptor_proto.name))
        depth -= 1
        args.cpp_file.write('{0}}}\n'.format(indent_str(depth)))
        depth -= 1
        args.cpp_file.write('{0}}}\n'.format(indent_str(depth)))
    else:
        if field_descriptor_proto.type is field_descriptor_proto.TYPE_STRING:
            args.cpp_file.write('{0}retval.{1} = string_from_bin(bin);\n'.format(indent_str(depth), field_descriptor_proto.name))
        elif field_descriptor_proto.type is field_descriptor_proto.TYPE_ENUM:
            args.cpp_file.write('{0}retval.{1} = enum_from_bin<std::remove_reference<decltype(retval.{1})>::type>(bin);\n'.format(indent_str(depth), field_descriptor_proto.name, to_cpp_namespace(leaf_type)))
        else:
            args.cpp_file.write('{0}retval.{1} = {2}_from_bin(bin);\n'.format(indent_str(depth), field_descriptor_proto.name, field_type.replace('::', '_')))

def define_cpp_from_bin_impl(decl_data, depth, map_fields):
    args.cpp_file.write('''
{0}{1}
{0}{{
'''.format(indent_str(depth), make_decl(decl_data, 'from+bin+cpp')))
    descriptor_proto = decl_data['descriptor_proto']
    depth += 1
    args.cpp_file.write('{0}{1} retval;\n'.format(indent_str(depth), make_return_type(decl_data, 'from+bin+cpp')))
    for field_descriptor_proto in descriptor_proto.field:
        define_cpp_from_bin_impl_field(field_descriptor_proto, depth, map_fields)
    args.cpp_file.write('{0}return retval;\n'.format(indent_str(depth)))
    depth -= 1
    args.cpp_file.write('{0}}}\n'.format(indent_str(depth)))

def define_csharp_bin_size_field(field_descriptor_proto, depth, map_fields):
    leaf_type = type_without_namespace(field_descriptor_proto, protobuf_namespace)
    field_type = field_element_type(field_descriptor_proto, 'cs')
    if repeated(field_descriptor_proto):
        args.cs_file.write('{0}retval += sizeof(int); '.format(indent_str(depth)))
        args.cs_file.write('foreach (var x in value.{1}) {{\n'.format(indent_str(depth), field_descriptor_proto.name))
        depth += 1
        if leaf_type in map_fields:
            key_field = map_fields[leaf_type].field[0]
            key_field_type = field_element_type(key_field, 'cs')
            if key_field.type is key_field.TYPE_STRING:
                args.cs_file.write('{0}retval += sizeof(int) + x.Key.Length;\n'.format(indent_str(depth)))
            else:
                if key_field.type is key_field.TYPE_ENUM:
                    key_field_type = 'int'
                args.cs_file.write('{0}retval += sizeof({1});\n'.format(indent_str(depth), key_field_type))
            value_field = map_fields[leaf_type].field[1]
            value_field_type = field_element_type(value_field, 'cs')
            if value_field.type is value_field.TYPE_MESSAGE:
                args.cs_file.write('{0}retval += size(x.Value);\n'.format(indent_str(depth)))
            elif value_field.type is value_field.TYPE_STRING:
                args.cs_file.write('{0}retval += sizeof(int) + x.Value.Length;\n'.format(indent_str(depth)))
            else:
                if value_field.type is value_field.TYPE_ENUM:
                    value_field_type = 'int'
                args.cs_file.write('{0}retval += sizeof({1});\n'.format(indent_str(depth), value_field_type))
        elif field_descriptor_proto.type is field_descriptor_proto.TYPE_MESSAGE:
            args.cs_file.write('{0}retval += size(x);\n'.format(indent_str(depth)))
        elif field_descriptor_proto.type is field_descriptor_proto.TYPE_STRING:
            args.cs_file.write('{0}retval += sizeof(int) + x.Length;\n'.format(indent_str(depth)))
        else:
            if field_descriptor_proto.type is field_descriptor_proto.TYPE_ENUM:
                field_type = 'int'
            args.cs_file.write('{0}retval += sizeof({1});\n'.format(indent_str(depth), field_type))
        depth -= 1
        args.cs_file.write('{0}}}\n'.format(indent_str(depth)))
    else:
        if field_descriptor_proto.type is field_descriptor_proto.TYPE_MESSAGE:
            args.cs_file.write('{0}retval += size(value.{1});\n'.format(indent_str(depth), field_descriptor_proto.name))
        elif field_descriptor_proto.type is field_descriptor_proto.TYPE_STRING:
            args.cs_file.write('{0}retval += sizeof(int) + value.{1}.Length;\n'.format(indent_str(depth), field_descriptor_proto.name))
        else:
            if field_descriptor_proto.type is field_descriptor_proto.TYPE_ENUM:
                field_type = 'int'
            args.cs_file.write('{0}retval += sizeof({1});\n'.format(indent_str(depth), field_type))

def define_csharp_bin_size(decl_data, depth, map_fields):
    args.cs_file.write('''{0}public static {1}
{0}{{
'''.format(indent_str(depth), make_decl(decl_data, 'to+size+cs')))
    depth += 1
    args.cs_file.write('{0}int retval = 0;\n'.format(indent_str(depth)))
    for field_descriptor_proto in decl_data['descriptor_proto'].field:
        define_csharp_bin_size_field(field_descriptor_proto, depth, map_fields)
    args.cs_file.write('{0}return retval;\n'.format(indent_str(depth)))
    depth -= 1
    args.cs_file.write('{0}}}\n\n'.format(indent_str(depth)))

def define_csharp_from_field(field_descriptor_proto, depth, map_fields):
    leaf_type = type_without_namespace(field_descriptor_proto, protobuf_namespace)
    field_type = field_element_type(field_descriptor_proto, 'cs')
    if repeated(field_descriptor_proto):
        args.cs_file.write('{0}{{\n'.format(indent_str(depth)))
        depth += 1
        args.cs_file.write('{0}int size = int_from_bin(bin, ref offset);\n'.format(indent_str(depth)))
        if leaf_type in map_fields:
            key_field = map_fields[leaf_type].field[0]
            key_field_type = field_element_type(key_field, 'cs')
            value_field = map_fields[leaf_type].field[1]
            value_field_type = field_element_type(value_field, 'cs')
            args.cs_file.write('{0}retval.{1} = new Dictionary<{2}, {3}>();\n'.format(indent_str(depth), field_descriptor_proto.name, key_field_type, value_field_type))
        else:
            args.cs_file.write('{0}retval.{1} = new {2}[size];\n'.format(indent_str(depth), field_descriptor_proto.name, field_type))

        args.cs_file.write('{0}foreach (var i in Enumerable.Range(0, size)) {{\n'.format(indent_str(depth)))
        depth += 1
        if leaf_type in map_fields:
            key_field = map_fields[leaf_type].field[0]
            key_field_type = field_element_type(key_field, 'cs')
            if key_field.type is key_field.TYPE_STRING:
                args.cs_file.write('{0}string key = string_from_bin(bin, ref offset);\n'.format(indent_str(depth)))
            elif key_field.type is key_field.TYPE_ENUM:
                args.cs_file.write('{0}{1} key = enum_from_bin<{1}>(bin, ref offset);\n'.format(indent_str(depth), key_field_type))
            else:
                args.cs_file.write('{0}{1} key = {1}_from_bin(bin, ref offset);\n'.format(indent_str(depth), key_field_type))
            value_field = map_fields[leaf_type].field[1]
            value_field_type = field_element_type(value_field, 'cs')
            if value_field.type is value_field.TYPE_STRING:
                args.cs_file.write('{0}string value = string_from_bin(bin, ref offset);\n'.format(indent_str(depth)))
            elif value_field.type is value_field.TYPE_ENUM:
                args.cs_file.write('{0}{1} value = enum_from_bin<{1}>(bin, ref offset);\n'.format(indent_str(depth), value_field_type))
            else:
                args.cs_file.write('{0}{1} value = {2}_from_bin(bin, ref offset);\n'.format(indent_str(depth), value_field_type, value_field_type.replace('.', '_')))
            args.cs_file.write('{0}retval.{1}[key] = value;\n'.format(indent_str(depth), field_descriptor_proto.name))
        else:
            if field_descriptor_proto.type is field_descriptor_proto.TYPE_STRING:
                args.cs_file.write('{0}string x = string_from_bin(bin, ref offset);\n'.format(indent_str(depth)))
            elif field_descriptor_proto.type is field_descriptor_proto.TYPE_ENUM:
                args.cs_file.write('{0}{1} x = enum_from_bin<{1}>(bin, ref offset);\n'.format(indent_str(depth), field_type))
            else:
                args.cs_file.write('{0}{1} x = {2}_from_bin(bin, ref offset);\n'.format(indent_str(depth), field_type, field_type.replace('.', '_')))
            args.cs_file.write('{0}retval.{1}[i] = x;\n'.format(indent_str(depth), field_descriptor_proto.name))
        depth -= 1
        args.cs_file.write('{0}}}\n'.format(indent_str(depth)))
        depth -= 1
        args.cs_file.write('{0}}}\n'.format(indent_str(depth)))
    else:
        if field_descriptor_proto.type is field_descriptor_proto.TYPE_MESSAGE:
            args.cs_file.write('{0}retval.{1} = {2}_from_bin(bin, ref offset);\n'.format(indent_str(depth), field_descriptor_proto.name, field_type.replace('.', '_')))
        elif field_descriptor_proto.type is field_descriptor_proto.TYPE_STRING:
            args.cs_file.write('{0}retval.{1} = string_from_bin(bin, ref offset);\n'.format(indent_str(depth), field_descriptor_proto.name))
        elif field_descriptor_proto.type is field_descriptor_proto.TYPE_ENUM:
            args.cs_file.write('{0}retval.{1} = enum_from_bin<{2}>(bin, ref offset);\n'.format(indent_str(depth), field_descriptor_proto.name, field_type))
        else:
            args.cs_file.write('{0}retval.{1} = {2}_from_bin(bin, ref offset);\n'.format(indent_str(depth), field_descriptor_proto.name, field_type))

def define_csharp_from(decl_data, depth, map_fields):
    args.cs_file.write('''{0}public static {1}
{0}{{
'''.format(indent_str(depth), make_decl(decl_data, 'from+bin+cs')))
    depth += 1
    args.cs_file.write('{0}{1} retval = new {1}();\n'.format(indent_str(depth), make_return_type(decl_data, 'from+bin+cs')))
    for field_descriptor_proto in decl_data['descriptor_proto'].field:
        define_csharp_from_field(field_descriptor_proto, depth, map_fields)
    args.cs_file.write('{0}return retval;\n'.format(indent_str(depth)))
    depth -= 1
    args.cs_file.write('{0}}}\n\n'.format(indent_str(depth)))

def define_csharp_to(decl_data, depth, map_fields):
    args.cs_file.write('''{0}public static {1}
{0}{{
'''.format(indent_str(depth), make_decl(decl_data, 'to+bin+cs')))
    depth += 1
    args.cs_file.write('{0}// TODO\n'.format(indent_str(depth)))
    depth -= 1
    args.cs_file.write('{0}}}\n\n'.format(indent_str(depth)))

user_namespace = args.namespace and args.namespace.split('::') or []
cs_namespace = user_namespace
if args.cs_namespace:
    cs_namespace = args.cs_namespace.split('.')

if args.cs_file and len(cs_namespace) == 0:
    raise Exception('When --cs-file is defined, a nonempty --cs-namespace must also be provided.')

file_descriptor_set = descriptor_pb2.FileDescriptorSet()
file_descriptor_set.ParseFromString(args.desc_file.read())
#print file_descriptor_set
for field_descriptor_proto in file_descriptor_set.file:
    proto_source = field_descriptor_proto.name
    syntax = str(field_descriptor_proto.syntax)
    protobuf_namespace = str(field_descriptor_proto.package).split('.')

    deps = map(lambda x: x[:-6], field_descriptor_proto.dependency)

    add_header_comment_and_includes(proto_source, syntax, deps)

    if args.cs_file:
        cs_deps = ';\n'.join(map(lambda x: 'using {}'.format(to_csharp_namespace(x)), deps))
        args.cs_file.write('''// WARNING: Generated code.
// This file was generated from {} ({})

using System;
using System.Text;
using System.Collections.Generic;

{}
namespace {} {{
'''.format(proto_source, syntax, cs_deps, '.'.join(cs_namespace)))

    depth = open_namespace(user_namespace)

    for enum_descriptor_proto in field_descriptor_proto.enum_type:
        handle_enum_descriptor_proto(enum_descriptor_proto, depth)

    all_decl_data = []
    map_fields = {}
    for descriptor_proto in field_descriptor_proto.message_type:
        declare_descriptor_proto(descriptor_proto, protobuf_namespace, user_namespace, depth, [], all_decl_data, map_fields)

    if args.cs_file:
        args.cs_file.write('''
public class convert
{

    private static Encoding ascii_encoding = Encoding.ASCII;

    private static bool bool_from_bin(byte[] bin, ref int offset)
    {
        bool retval = bin[offset] == 0 ? false : true;
        offset += 1;
        return retval;
    }

    private static int int_from_bin(byte[] bin, ref int offset)
    {
        int retval =
            (bin[offset + 0] << 24) +
            (bin[offset + 1] << 16) +
            (bin[offset + 2] <<  8) +
            (bin[offset + 3] <<  0);
        offset += 4;
        return retval;
    }

    private static float float_from_bin(byte[] bin, ref int offset)
    {
        float retval = BitConverter.ToSingle(bin, offset);
        offset += sizeof(float);
        return retval;
    }

    private static double double_from_bin(byte[] bin, ref int offset)
    {
        double retval = BitConverter.ToDouble(bin, offset);
        offset += sizeof(double);
        return retval;
    }

    private static T enum_from_bin<T>(byte[] bin, ref int offset)
    {
        int i = int_from_bin(bin, ref offset);
        return (T)Enum.ToObject(typeof(T), i);
    }

    private static string string_from_bin(byte[] bin, ref int offset)
    {
        int length = int_from_bin(bin, ref offset);
        string retval = ascii_encoding.GetString(bin, offset, length);
        offset += length;
        return retval;
    }

    private static void to_bin(bool b, byte[] bin, ref int offset)
    {
        bin[offset] = (byte)(b == true ? 1 : 0);
        offset += 1;
    }

    private static void to_bin(int i, byte[] bin, ref int offset)
    {
        bin[offset + 0] = (byte)((i >> 24) & 0xff);
        bin[offset + 1] = (byte)((i >> 16) & 0xff);
        bin[offset + 2] = (byte)((i >>  8) & 0xff);
        bin[offset + 3] = (byte)((i >>  0) & 0xff);
        offset += 4;
    }

    private static void to_bin(float f, byte[] bin, ref int offset)
    {
        foreach (byte b in BitConverter.GetBytes(f)) {
            bin[offset++] = b;
        }
    }

    private static void to_bin(double d, byte[] bin, ref int offset)
    {
        foreach (byte b in BitConverter.GetBytes(d)) {
            bin[offset++] = b;
        }
    }

    private static void to_bin(string s, byte[] bin, ref int offset)
    {
        int length = ascii_encoding.GetByteCount(s);
        to_bin(length, bin, ref offset);
        foreach (byte b in ascii_encoding.GetBytes(s)) {
            bin[offset++] = b;
        }
    }

''')

    close_namespace(user_namespace)

    args.cpp_file.write('''
namespace {

    bool bool_from_bin (unsigned char*& bin)
    {
        bool retval = static_cast<bool>(*bin);
        bin += 1;
        return retval;
    }

    int int_from_bin (unsigned char*& bin)
    {
        int retval =
            (bin[0] << 24) +
            (bin[1] << 16) +
            (bin[2] <<  8) +
            (bin[3] <<  0);
        bin += 4;
        return retval;
    }

    float float_from_bin (unsigned char*& bin)
    {
        float retval;
        memcpy(&retval, bin, sizeof(float));
        bin += sizeof(float);
        return retval;
    }

    double double_from_bin (unsigned char*& bin)
    {
        double retval;
        memcpy(&retval, bin, sizeof(double));
        bin += sizeof(double);
        return retval;
    }

    template <typename T>
    T enum_from_bin (unsigned char*& bin)
    {
        int i = int_from_bin(bin);
        return static_cast<T>(i);
    }

    std::string string_from_bin (unsigned char*& bin)
    {
        std::size_t length = int_from_bin(bin);
        std::string retval((const char*)bin, length);
        bin += length;
        return retval;
    }

    void to_bin (bool b, std::vector<unsigned char>& bin)
    {
        bin.push_back(static_cast<unsigned char>(b));
    }

    void to_bin (int i, std::vector<unsigned char>& bin)
    {
        bin.push_back(static_cast<unsigned char>((i >> 24) & 0xff));
        bin.push_back(static_cast<unsigned char>((i >> 16) & 0xff));
        bin.push_back(static_cast<unsigned char>((i >>  8) & 0xff));
        bin.push_back(static_cast<unsigned char>((i >>  0) & 0xff));
    }

    void to_bin (float f, std::vector<unsigned char>& bin)
    {
        bin.resize(bin.size() + sizeof(float));
        memcpy(&bin[bin.size() - sizeof(float)], &f, sizeof(float));
    }

    void to_bin (double d, std::vector<unsigned char>& bin)
    {
        bin.resize(bin.size() + sizeof(double));
        memcpy(&bin[bin.size() - sizeof(double)], &d, sizeof(double));
    }

    template <typename T>
    void to_bin (T e, std::vector<unsigned char>& bin)
    {
        int i = static_cast<int>(e);
        to_bin(i, bin);
    }

    void to_bin (const std::string s, std::vector<unsigned char>& bin)
    {
        int length = static_cast<int>(s.size());
        to_bin(length, bin);
        bin.resize(bin.size() + length);
        std::copy(s.begin(), s.end(), bin.end() - length);
    }

}

''')

    for i in range(len(all_decl_data)):
        args.hpp_file.write('''
{0}{1};
{0}{2};
'''.format(indent_str(depth), make_decl(all_decl_data[i], 'to+pb+cpp'), make_decl(all_decl_data[i], 'from+pb+cpp')))
        args.hpp_file.write('''
{0}{1};
{0}{2};
'''.format(indent_str(depth), make_decl(all_decl_data[i], 'to+bin+cpp'), make_decl(all_decl_data[i], 'from+bin+cpp')))

        define_cpp_to_pb_impl(all_decl_data[i], depth, map_fields)
        define_cpp_from_pb_impl(all_decl_data[i], depth, map_fields)
        define_cpp_to_bin_impl(all_decl_data[i], depth, map_fields)
        define_cpp_from_bin_impl(all_decl_data[i], depth, map_fields)

        if args.cs_file:
            define_csharp_bin_size(all_decl_data[i], 1, map_fields)
            define_csharp_from(all_decl_data[i], 1, map_fields)
            define_csharp_to(all_decl_data[i], 1, map_fields)

    if args.cs_file:
        args.cs_file.write('};\n\n}\n')
