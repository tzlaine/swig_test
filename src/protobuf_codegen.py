#!/usr/bin/env python

import argparse

parser = argparse.ArgumentParser(description='Generate C++ structs and binding layers from Protobuf messages.')
parser.add_argument('--proto-file', required=True, type=argparse.FileType('r'),
                    help='Original .proto file')
parser.add_argument('--desc-file', required=True, type=argparse.FileType('rb'),
                    help='Input proto description file (binary)')
parser.add_argument('--namespace', type=str,
                    help='Namespace into which to place generated code (e.g. foo::bar).')
parser.add_argument('--file-root', required=True,
                    help='Output .cpp file')
args = parser.parse_args()

from google.protobuf import descriptor_pb2
import re
import os

cpp_file = open(args.file_root + '.cpp', 'w')
hpp_file = open(args.file_root + '.hpp', 'w')
formatters_file = open(args.file_root + '_formatters.hpp', 'w')
serialization_file = open(args.file_root + '_serialization.hpp', 'w')
metadata_file = open(args.file_root + '_metadata.hpp', 'w')
enum_lua_bindings_file = open(args.file_root + '_enum_lua_bindings.hpp', 'w')

proto_contents = args.proto_file.read()

# Tuples of (c++-type, c#-type).  Using 'void' for several of these
# that are not yet supported, to cause compilation failures in the
# generated types.  Comments below from descriptor.proto in the
# Protobuf sources.
types = [
    ('void', 'void'),            #    // 0 is reserved for errors.
    ('double', 'double'),        #    TYPE_DOUBLE         = 1;
    ('float', 'float'),          #    TYPE_FLOAT          = 2;
    ('std::ptrdiff_t', 'int'),   #    TYPE_INT64          = 3;
    ('std::size_t', 'uint'),     #    TYPE_UINT64         = 4;
    ('int', 'int'),              #    TYPE_INT32          = 5;
    ('void', 'void'),            #    TYPE_FIXED64        = 6;
    ('int', 'int'),              #    TYPE_FIXED32        = 7;
    ('bool', 'bool'),            #    TYPE_BOOL           = 8;
    ('adobe::name_t', 'string'), #    TYPE_STRING         = 9;
    ('void', 'void'),            #    TYPE_GROUP          = 10;  // Tag-delimited aggregate.
    ('void', 'void'),            #    TYPE_MESSAGE        = 11;  // Length-delimited aggregate.
    ('void', 'void'),            #    TYPE_BYTES          = 12;
    ('unsigned int', 'uint'),    #    TYPE_UINT32         = 13;
    ('void', 'void'),            #    TYPE_ENUM           = 14;
    ('int', 'int'),              #    TYPE_SFIXED32       = 15;
    ('void', 'void'),            #    TYPE_SFIXED64       = 16;
    ('int', 'int'),              #    TYPE_SINT32         = 17;  // Uses ZigZag encoding.
    ('void', 'void')             #    TYPE_SINT64         = 18;  // Uses ZigZag encoding.
]

def get_cpp_type(typecode):
    retval = 'void'
    if typecode < len(types):
        retval = types[typecode][0]
    return retval

def map_field_entry_type(descriptor_proto):
    return descriptor_proto.options.map_entry == True

def type_without_namespace(field_descriptor_proto, namespace):
    typenames = field_descriptor_proto.type_name.split('.')[1:]
    prefix = list(filter(lambda x: x[0] == x[1], zip(typenames, namespace)))
    return '.'.join(typenames[len(prefix):])

def type_namespace(field_descriptor_proto, namespace):
    typenames = field_descriptor_proto.type_name.split('.')[1:]
    prefix = list(filter(lambda x: x[0] == x[1], zip(typenames, namespace)))
    return '.'.join(typenames[:len(prefix)])

def to_cpp_namespace(typename):
    return typename.replace('.', '::')

def indent_str(i):
    return '    ' * i

def indent(i):
    cpp_file.write(indent_str(i))
    hpp_file.write(indent_str(i))
    formatters_file.write(indent_str(i))
    serialization_file.write(indent_str(i))
    metadata_file.write(indent_str(i))
    enum_lua_bindings_file.write(indent_str(i))

def newline():
    cpp_file.write('\n')
    hpp_file.write('\n')
    formatters_file.write('\n')
    serialization_file.write('\n')
    metadata_file.write('\n')
    enum_lua_bindings_file.write('\n')

def add_header_comment_and_includes(proto_source, syntax, deps):
    cpp_deps = '\n'.join(map(lambda x: '#include <{}.hpp>'.format(x), deps))
    formatters_deps = cpp_deps.replace('.hpp', '_formatters.hpp')
    serialization_deps = cpp_deps.replace('.hpp', '_serialization.hpp')
    metadata_deps = cpp_deps.replace('.hpp', '_metadata.hpp')

    cpp_file.write('''// WARNING: Generated code.
// This file was generated from {} ({})

#ifndef BUILD_FOR_TEST
#include "Spaceport.h"
#endif

#include "{}"
{}


'''.format(proto_source, syntax, os.path.basename(hpp_file.name), cpp_deps))

    hpp_file.write('''// WARNING: Generated code.
// This file was generated from {0} ({1})
#pragma once

#include "config.hpp"
{2}

#include <flags.hpp>

#include <string>
#include <vector>
#include <adobe/name.hpp>
#include <boost/container/flat_map.hpp>


'''.format(os.path.splitext(proto_source)[0], syntax, cpp_deps))

    formatters_file.write('''// WARNING: Generated code.
// This file was generated from {} ({})
#pragma once

#include "{}"
{}
{}

#include <format>
#if defined(BUILD_FOR_TEST)
#include <iosfwd>
#endif

'''.format(proto_source, syntax, os.path.basename(hpp_file.name), cpp_deps, formatters_deps))

    serialization_file.write('''// WARNING: Generated code.
// This file was generated from {} ({})
#pragma once

#include "{}"
{}
{}

#include <serialization.hpp>
#if defined(BUILD_FOR_TEST)
#include <iosfwd>
#endif

namespace detail {{
'''.format(proto_source, syntax, os.path.basename(hpp_file.name), cpp_deps, serialization_deps))

    metadata_file.write('''// WARNING: Generated code.
// This file was generated from {} ({})
#pragma once

#include <metadata.hpp>

#include "{}"
{}
{}

namespace detail {{
'''.format(proto_source, syntax, os.path.basename(hpp_file.name), cpp_deps, metadata_deps))

    enum_lua_bindings_file.write('''// WARNING: Generated code.
// This file was generated from {} ({})
#pragma once

#include "lua.hpp"

#include "{}"
{}

namespace detail {{

    inline void {}_register_enums(sol::state & l)
    {{
'''.format(proto_source, syntax, os.path.basename(hpp_file.name), cpp_deps, os.path.basename(hpp_file.name)[:-4]))

def open_namespace(namespace, depth=0):
    indent(depth)
    for name in namespace:
        cpp_file.write('namespace {} {{ '.format(name))
        hpp_file.write('namespace {} {{ '.format(name))
        metadata_file.write('namespace {} {{ '.format(name))
        formatters_file.write('namespace {} {{ '.format(name))
    newline()
    return depth + (len(namespace) and 1 or 0)

def close_namespace(namespace, depth=0):
    indent(depth)
    newline()
    for name in namespace:
        cpp_file.write('} ')
        hpp_file.write('} ')
        metadata_file.write('} ')
        formatters_file.write('} ')
    newline()

def write_formatter_switch(enum_descriptor_proto, depth, path, printing_for_user):
    predicate = ''
    if printing_for_user is not None:
        predicate = printing_for_user and 'if (printing_for_user_) ' or 'if (!printing_for_user_) '
    formatters_file.write('''{0}    {1}switch (t) {{
'''.format(indent_str(depth), predicate))

    index = 0
    for enum_value_descriptor_proto in enum_descriptor_proto.value:
        new_path = path + [field_path_constant, index]
        index += 1
        if enum_value_descriptor_proto.name == f'invalid_{enum_descriptor_proto.name[:-2]}':
            formatters_file.write('{0}        case {1}::{2}: name = "INVALID"sv; break;\n'.format(indent_str(depth), enum_descriptor_proto.name, enum_value_descriptor_proto.name))
        else:
            string_to_print = enum_value_descriptor_proto.name
            if printing_for_user is True and tuple(new_path) in special_comments and \
               special_comments[tuple(new_path)][0] == user_string_constant:
                string_to_print = special_comments[tuple(new_path)][1]
            formatters_file.write('{0}        case {1}::{2}: name = "{3}"sv; break;\n'.format(indent_str(depth), enum_descriptor_proto.name, enum_value_descriptor_proto.name, string_to_print))

    formatters_file.write('{0}    }}\n'.format(indent_str(depth)))

def handle_enum_descriptor_proto(enum_descriptor_proto, depth, path):
    hpp_file.write('\n')

    flags_enum = tuple(path) in special_comments and special_comments[tuple(path)][0] == flags_enum_constant

    hpp_file.write('''{}enum class {} {}{{
'''.format(indent_str(depth), enum_descriptor_proto.name, flags_enum and ': unsigned int ' or ''))
    enum_lua_bindings_file.write('''{0}auto {1}_enum = l.new_enum<{1}>("{1}", {{
'''.format(indent_str(depth + 2), enum_descriptor_proto.name))

    # If we see even one U"foo" comment, we know we need to have two printing
    # modes -- one for debug dumps, and one for users.
    user_strings_available = False
    for i in range(len(enum_descriptor_proto.value)):
        curr_path = tuple(path + [field_path_constant, i])
        if curr_path in special_comments and special_comments[curr_path][0] == user_string_constant:
            user_strings_available = True
            break

    formatters_file.write('''template <>
{0}struct std::formatter<{1}> : std::formatter<std::string_view> {{
'''.format(indent_str(depth), enum_descriptor_proto.name))
    if user_strings_available:
        formatters_file.write('''{0}    template<class ParseContext>
{0}    constexpr auto parse(ParseContext & ctx) {{
{0}        auto f = ctx.begin();
{0}        auto const l = ctx.end();
{0}        if (f != l && *f == 'u') {{
{0}            ++f;
{0}            printing_for_user_ = true;
{0}        }}
{0}        if (f != l && *f != '}}')
{0}            throw std::format_error("Invalid format specifier.");
{0}        return f;
{0}    }}
{0}    bool printing_for_user_ = false;
'''.format(indent_str(depth)))
    formatters_file.write('''{0}    template <typename FormatContext>
{0}    auto format({1} t, FormatContext & ctx) const {{
{0}        std::string_view name;
{0}        using namespace std::literals;
'''.format(indent_str(depth), enum_descriptor_proto.name))

    depth += 1

    first = True
    ored_enumerators = ''
    for enum_value_descriptor_proto in enum_descriptor_proto.value:
        hpp_file.write('{}{} = {},\n'.format(indent_str(depth), enum_value_descriptor_proto.name, enum_value_descriptor_proto.number))
        if first:
            ored_enumerators = f'flags({enum_descriptor_proto.name}::{enum_value_descriptor_proto.name})'
            first = False
        else:
            ored_enumerators += f' | {enum_descriptor_proto.name}::{enum_value_descriptor_proto.name}'
        enum_lua_bindings_file.write('''{0}{{"{1}", {2}::{1}}},
'''.format(indent_str(depth + 3), enum_value_descriptor_proto.name, enum_descriptor_proto.name))

    if user_strings_available:
        write_formatter_switch(enum_descriptor_proto, depth, path, True)
        write_formatter_switch(enum_descriptor_proto, depth, path, False)
    else:
        write_formatter_switch(enum_descriptor_proto, depth, path, None)

    depth -= 1
    hpp_file.write('''{0}}};
'''.format(indent_str(depth)))
    if flags_enum:
        hpp_file.write('''{0}template<> inline flags<{1}> all_flags<{1}>() {{ return {2}; }}
{0}inline flags<{1}> operator|({1} x, {1} y) {{ return flags(x) | y; }}
{0}inline flags<{1}> operator~({1} x) {{ return ~flags(x); }}
'''.format(indent_str(depth), enum_descriptor_proto.name, ored_enumerators))
    else:
        hpp_file.write('''{0}inline auto operator<=>({1} x, {1} y) {{ return (int)x <=> (int)y; }}
'''.format(indent_str(depth), enum_descriptor_proto.name))

    formatters_file.write('''{0}        return std::formatter<std::string_view>::format(name, ctx);
{0}    }}
{0}}};

'''.format(indent_str(depth)))

    enum_lua_bindings_file.write('''{0}}});

'''.format(indent_str(depth + 2)))

def repeated(field_descriptor_proto):
    return field_descriptor_proto.label is field_descriptor_proto.LABEL_REPEATED

def field_element_type(field_descriptor_proto, lang):
    if field_descriptor_proto.type is field_descriptor_proto.TYPE_MESSAGE or \
       field_descriptor_proto.type is field_descriptor_proto.TYPE_ENUM:
        typename = type_without_namespace(field_descriptor_proto, protobuf_namespace)
        if lang == 'cpp':
            typename = typename.replace('.', '::')
    else:
        if lang == 'cpp':
            typename = get_cpp_type(field_descriptor_proto.type)
    return typename

def field_type(field_descriptor_proto, lang, map_fields):
    leaf_type = type_without_namespace(field_descriptor_proto, protobuf_namespace)
    typename = field_element_type(field_descriptor_proto, lang)
    if leaf_type in map_fields:
        key_type = field_type(map_fields[leaf_type].field[0], 'cpp', map_fields)
        value_type = field_type(map_fields[leaf_type].field[1], 'cpp', map_fields)
        typename = f'boost::container::flat_map<{key_type}, {value_type}>'
    elif repeated(field_descriptor_proto):
        if lang == 'cpp':
            typename = 'std::vector<{}>'.format(typename)
        else:
            typename = '{}[]'.format(typename)
    return typename

def initializer_expr(field_descriptor_proto, lang, map_fields):
    if field_descriptor_proto.type is field_descriptor_proto.TYPE_MESSAGE:
        return ' = {}'

    if field_descriptor_proto.type is field_descriptor_proto.TYPE_ENUM:
        typename = type_without_namespace(field_descriptor_proto, protobuf_namespace)
        return f' = {typename}::invalid_{typename[:-2]}'

    typename = field_type(field_descriptor_proto, lang, map_fields)
    if typename == 'nation_and_object_id_t':
        return ' = {-1, -1}'
    if typename == 'adobe::name_t':
        return ' = adobe::name_t("")'
    if typename == 'unsigned int':
        return ' = 0u - 1'
    if typename == 'std::size_t':
        return ' = std::size_t(0) - 1'
    if typename == 'int':
        return ' = -1'
    if typename == 'float':
        return ' = -1.0f'
    if typename == 'double':
        return ' = -1.0'
    if typename == 'bool':
        return ' = false'

    return ''

def declare_field_descriptor_proto(field_descriptor_proto, depth, map_fields, path):
    # TODO if tuple(path) in special_comments:
    # TODO     print(f'{field_descriptor_proto.name} has special comment {special_comments[tuple(path)]}')
    typename = field_type(field_descriptor_proto, 'cpp', map_fields)
    if 'boost::container::flat_map' in typename:
        hpp_file.write('{}{} {};\n'.format(indent_str(depth), typename, field_descriptor_proto.name))
        formatters_file.write('''{0}        out = std::format_to(out, " {1}={{{{");
'''.format(indent_str(depth - 1), field_descriptor_proto.name))
        formatters_file.write('''{0}        for (auto && [key, value] : x.{1}) {{
'''.format(indent_str(depth - 1), field_descriptor_proto.name))
        formatters_file.write('''{0}            out = std::format_to(out, " {{}}:{{}}", key, value);
'''.format(indent_str(depth - 1)))
        formatters_file.write('''{0}        }};
{0}        out = std::format_to(out, " }}}}");
'''.format(indent_str(depth - 1)))
    else:
        initializer = initializer_expr(field_descriptor_proto, 'cpp', map_fields)
        hpp_file.write('{}{} {}{};\n'.format(indent_str(depth), typename, field_descriptor_proto.name, initializer))
        if repeated(field_descriptor_proto):
            formatters_file.write('''{0}        out = std::format_to(out, " {1}=[");
'''.format(indent_str(depth - 1), field_descriptor_proto.name))
            formatters_file.write('''{0}        for (auto && e : x.{1}) {{
'''.format(indent_str(depth - 1), field_descriptor_proto.name))
            formatters_file.write('''{0}            out = std::format_to(out, " {{}}", e);
'''.format(indent_str(depth - 1)))
            formatters_file.write('''{0}        }};
{0}        out = std::format_to(out, " ]");
'''.format(indent_str(depth - 1)))
        else:
            formatters_file.write('{0}        out = std::format_to(out, " {1}={{}}", x.{1});\n'.format(indent_str(depth - 1), field_descriptor_proto.name))

def declare_descriptor_proto(descriptor_proto, protobuf_namespace, user_namespace, depth, scope, all_decl_data, map_fields, path):
    hpp_file.write('\n')
    hpp_file.write('{0}struct {1}\n{0}{{\n'.format(indent_str(depth), descriptor_proto.name))
    formatters_file.write('''{0}template <>
{0}struct std::formatter<{1}> {{
{0}    constexpr auto parse(std::format_parse_context & ctx)
{0}    {{ return ctx.begin(); }}
{0}
{0}    template <typename Ctx>
{0}    auto format({1} const & x, Ctx & ctx) const {{
{0}        auto out = ctx.out();
{0}        out = std::format_to(out, "{1}(");

'''.format(indent_str(depth), descriptor_proto.name))
    depth += 1

    index = 0
    for enum_descriptor_proto in descriptor_proto.enum_type:
        new_path = path + [enum_path_constant, index]
        index += 1
        handle_enum_descriptor_proto(enum_descriptor_proto, depth, new_path)

    index = 0
    for descriptor in descriptor_proto.nested_type:
        new_path = path + [message_path_constant, index]
        if map_field_entry_type(descriptor):
            field = None
            for f in descriptor_proto.field:
                if f.type_name.endswith(descriptor.name):
                    field = f
                    break
            name = type_without_namespace(field, protobuf_namespace)
            map_fields[name] = descriptor
        else:
            declare_descriptor_proto(descriptor, protobuf_namespace, user_namespace, depth, scope + [descriptor_proto.name], all_decl_data, map_fields, new_path)
        index += 1

    index = 0
    this_message_name = descriptor_proto.name
    this_message_fields = []
    for field_descriptor_proto in descriptor_proto.field:
        new_path = path + [field_path_constant, index]
        index += 1
        declare_field_descriptor_proto(field_descriptor_proto, depth, map_fields, new_path)
        this_message_fields.append((
            field_descriptor_proto.name,
            field_descriptor_proto.number,
            field_type(field_descriptor_proto, 'cpp', map_fields)))
    print_serialization(depth, this_message_name, this_message_fields)
    print_metadata(depth, this_message_name, this_message_fields)
    depth -= 1
    hpp_file.write('''{0}    bool operator==({1} const &) const = default;
{0}}};
'''.format(indent_str(depth), descriptor_proto.name))
    formatters_file.write('''
{0}        return std::format_to(out, " )");
{0}    }}
{0}}};
#if defined(BUILD_FOR_TEST)
{0}inline std::ostream & operator<<(std::ostream & os, {1} const & x)
{0}{{ return os << std::format("{{}}", x); }}
#endif

'''.format(indent_str(depth), descriptor_proto.name))
    decl_data = {
        'scope_ns': scope,
        'proto_ns': protobuf_namespace,
        'user_ns': user_namespace,
        'name': descriptor_proto.name,
        'descriptor_proto': descriptor_proto
    }
    all_decl_data.append(decl_data)

def print_serialization(depth, this_message_name, this_message_fields):
    serialize_ops = '\n'.join(map(lambda tup: f'''{indent_str(depth)}    if (std::ranges::none_of(elisions, [](int i) {{ return i == {tup[1]}; }}))
{indent_str(depth)}        retval += detail::serialize_impl<Op, ser_field_op::write>(x.{tup[0]}, {tup[1]}, os);''', this_message_fields))

    serialization_file.write('''{0}template<ser_op Op, ser_field_op FieldOp, typename OStream, int N = 1>
{0}std::ptrdiff_t serialize_message_impl({1} const & x, int field_number, OStream * os, std::array<int, N> const & elisions = {{{{0}}}})
{0}{{
{0}    std::ptrdiff_t retval = 0;
{0}
{0}    if constexpr (FieldOp == ser_field_op::write) {{
{0}        uint8_t buf[16];
{0}        uint8_t * out = buf;
{0}        out = os::WriteVarint32ToArray(field_number, out);
{0}        detail::count_or_write<Op>(retval, buf, out - buf, os);
{0}    }}
{0}
{2}
{0}
{0}    retval += detail::serialize_message_end<Op>(os);
{0}
{0}    return retval;
{0}}}
'''.format(indent_str(depth), this_message_name, serialize_ops))

    fields_dict = {}
    for tup in this_message_fields:
        fields_dict[tup[1]] = tup[0]

    lo = min(map(lambda tup: tup[1], this_message_fields))
    hi = max(map(lambda tup: tup[1], this_message_fields))

    unknown_name = '<UNKOWN_FIELD>'

    field_names = []
    for i in range(1, hi + 1):
        name = unknown_name
        if i in fields_dict:
            name = fields_dict[i]
        field_names.append(f'"{name}"sv')
    field_names = ', '.join(field_names)

    field_numbers = ', '.join(map(lambda tup: f'{tup[1]}', this_message_fields))
    deserialize_ops = '\n'.join(map(lambda tup: f'{indent_str(depth)}        case {tup[1]}: return detail::deserialize_impl(x.{tup[0]}, src);', this_message_fields))

    serialization_file.write('''{0}template<> inline std::span<std::byte const> deserialize_message_impl<{1}>({1} & x, std::span<std::byte const> src)
{0}{{
{0}    using namespace std::literals;
{0}    constexpr auto this_message_name = "{1}"sv;
{0}    constexpr std::array<std::string_view, {2}> field_names = {{{{"<UNKOWN_FIELD>"sv,
{0}      {3}}}}};
{0}    std::array<int, {4}> expected_field_numbers = {{{{
{0}      {5}}}}};
{0}
{0}    constexpr int lo_field_number = {6};
{0}    constexpr int hi_field_number = {7};
{0}
{0}    auto read_field = [] ({1} & x, int i, std::span<std::byte const> src) {{
{0}        switch (i) {{
{8}
{0}        default: return src; // unreachable
{0}        }}
{0}    }};
{0}
{0}    return detail::deserialize_message_impl_impl<lo_field_number, hi_field_number>(
{0}        x, src, this_message_name, field_names, expected_field_numbers, read_field);
{0}}}

'''.format(indent_str(depth), this_message_name, hi + 1, field_names, len(fields_dict), field_numbers, lo, hi, deserialize_ops))

def print_metadata(depth, this_message_name, this_message_fields):
    member_metadata = '\n'.join(map(lambda tup: f'''{indent_str(depth)}    static constexpr metadatum<{this_message_name}, {tup[2]}> {tup[0]}()
{indent_str(depth)}    {{
{indent_str(depth)}        using namespace std::literals;
{indent_str(depth)}        return {{"{tup[0]}"sv, {tup[1]}, &{this_message_name}::{tup[0]}}};
{indent_str(depth)}    }}''', this_message_fields))

    for_each_body = '\n'.join(map(lambda tup: f'{indent_str(depth)}        f({tup[0]}());', this_message_fields))

    lo = min(map(lambda tup: tup[1], this_message_fields))
    hi = max(map(lambda tup: tup[1], this_message_fields))

    metadata_file.write('''{0}template<> struct metadata<{1}>
{0}{{
{0}    static constexpr std::string_view struct_name()
{0}    {{
{0}        using namespace std::literals;
{0}        return "{1}"sv;
{0}    }}
{0}    static constexpr int lo_field_number() {{ return {2}; }}
{0}    static constexpr int hi_field_number() {{ return {3}; }}

{4}

{0}    template<typename F>
{0}    static void foreach_member(F && f)
{0}    {{
{5}
{0}    }}
{0}}};

'''.format(indent_str(depth), this_message_name, lo, hi, member_metadata, for_each_body))

user_namespace = args.namespace and args.namespace.split('::') or []

file_descriptor_set = descriptor_pb2.FileDescriptorSet()
file_descriptor_set.ParseFromString(args.desc_file.read())

user_string_constant = 0
flags_enum_constant = 1

# First pass: gather special comments and apths to each.
import re
user_string_comment_regex = re.compile(r'U"(.+)"')
flags_enum_comment_regex = re.compile(r'(FLAGS)')
special_comments = {}
for field_descriptor_proto in file_descriptor_set.file:
    source_code_info = field_descriptor_proto.source_code_info
    for location in source_code_info.location:
        if location.leading_comments:
            #print('*' * 80)
            #print('FOUND LEADING COMMENT {} at {}', location.leading_comments, tuple(location.path))
            match_ = flags_enum_comment_regex.search(location.leading_comments)
            if match_:
                special_comments[tuple(location.path)] = (flags_enum_constant, match_.group(1))
        if location.trailing_comments:
            # print('*' * 80)
            # print('FOUND TRAILING COMMENT {} at {}', location.trailing_comments, tuple(location.path))
            match_ = user_string_comment_regex.search(location.trailing_comments)
            if match_:
                special_comments[tuple(location.path)] = (user_string_constant, match_.group(1))

message_path_constant = 4
enum_path_constant = 5
field_path_constant = 2

# Second pass: this is the main effort.
for field_descriptor_proto in file_descriptor_set.file:
    proto_source = field_descriptor_proto.name
    syntax = str(field_descriptor_proto.syntax)
    protobuf_namespace = str(field_descriptor_proto.package).split('.')

    deps = map(lambda x: x[:-6], field_descriptor_proto.dependency)

    add_header_comment_and_includes(proto_source, syntax, deps)

    depth = open_namespace(user_namespace)

    index = 0;
    for enum_descriptor_proto in field_descriptor_proto.enum_type:
        handle_enum_descriptor_proto(enum_descriptor_proto, depth, [enum_path_constant, index])
        index += 1

    all_decl_data = []
    map_fields = {}
    index = 0;
    for descriptor_proto in field_descriptor_proto.message_type:
        declare_descriptor_proto(descriptor_proto, protobuf_namespace, user_namespace, depth, [], all_decl_data, map_fields, [message_path_constant, index])
        index += 1

    serialization_file.write('}\n')
    metadata_file.write('}\n')
    enum_lua_bindings_file.write('    }\n}\n')

    close_namespace(user_namespace)
