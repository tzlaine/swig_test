#pragma once

#include <string>


struct name_and_index
{
    std::string_view name_;
    int index_;
};

template<typename Message, typename T>
struct metadatum : name_and_index
{
    T Message::* ptr_;
};

template<typename T>
struct metadata
{};
