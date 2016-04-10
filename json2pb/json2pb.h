/*
 * Copyright (c) 2013 Pavel Shramov <shramov@mexmat.net>
 *
 * json2pb is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See LICENSE for details.
 */

#ifndef __JSON2PB_H__
#define __JSON2PB_H__

#include <string>

namespace google { namespace protobuf {
    class Message;
} }

enum class map_encoding_t {
    compact, // "foo": { ... } for each map element
    verbose  // { "key": "foo", "value": { ... } } for each map element
};

enum class whitespace_t {
    indented,
    minified
};

void json2pb(google::protobuf::Message &msg,
             const char *buf,
             size_t size,
             map_encoding_t map_encoding);

std::string pb2json(const google::protobuf::Message &msg,
                    map_encoding_t map_encoding,
                    whitespace_t whitespace);
void pb2json(const google::protobuf::Message &msg,
             std::string& r,
             map_encoding_t map_encoding,
             whitespace_t whitespace);

#endif
