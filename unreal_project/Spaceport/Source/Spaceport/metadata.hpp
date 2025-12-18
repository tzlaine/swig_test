#pragma once

#include <string>


namespace detail {
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
}
