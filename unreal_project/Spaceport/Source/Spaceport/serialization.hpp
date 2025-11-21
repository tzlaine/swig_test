#pragma once

#include "logging.hpp"
#include "memmap.hpp"

#include <adobe/name.hpp>
#include <boost/container/flat_map.hpp>
#include <google/protobuf/io/coded_stream.h>

#include <array>
#include <exception>
#include <filesystem>
#include <ranges>
#include <string_view>
#include <type_traits>
#include <vector>
#include <cassert>


struct failed_deserialization : std::exception
{
    failed_deserialization(std::string initial_msg)
    {
        msg_parts_.push_back(std::move(initial_msg));
        rebuild_msg();
    }

    void add_info(std::string msg)
    {
        msg_parts_.push_back(std::move(msg_));
        rebuild_msg();
    }

    char const * what() const noexcept { return msg_.c_str(); }

private:
    void rebuild_msg()
    {
        msg_ = "Deserialization failed.  Context:";
        for (auto const & part : msg_parts_ | std::views::reverse) {
            msg_ += "\n  ";
            msg_ += part;
        }
    }

    std::vector<std::string> msg_parts_;
    std::string msg_;
};

namespace detail {
    template<typename T>
    constexpr bool is_vector_v = false;
    template<typename T>
    constexpr bool is_vector_v<std::vector<T>> = true;

    template<typename T>
    constexpr bool is_map_v = false;
    template<typename K, typename V>
    constexpr bool is_map_v<boost::container::flat_map<K, V>> = true;

    using os = google::protobuf::io::CodedOutputStream;
    using is = google::protobuf::io::CodedInputStream;

    enum struct ser_op { write, count };
    enum struct ser_field_op { write, dont_write };

    template<ser_op Op, typename Char>
    void count_or_write(
        std::ptrdiff_t & bytes_written,
        Char const * data,
        int size,
        std::ostream * os)
    {
        if constexpr (Op == ser_op::write)
            os->write(reinterpret_cast<char const *>(data), size);
        else
            bytes_written += size;
    }

#if 0 // TODO: Use these from 'os' above instead of actually doing the
      // serialization, if it's faster to do so.
    static size_t VarintSize32(uint32 value);
    static size_t VarintSize64(uint64 value);
    static size_t VarintSize32SignExtended(int32 value);
#endif

    template<ser_op Op, ser_field_op FieldOp, typename T>
    std::ptrdiff_t
    serialize_impl(T const & x, int field_number, std::ostream * os)
    {
        std::ptrdiff_t retval = 0;

        uint8_t buf[64];
        uint8_t * out = buf;

        if constexpr (std::is_same_v<T, bool>) {
            buf[0] = x;
            detail::count_or_write<Op>(retval, buf, 1, os);
        } else if constexpr (std::is_enum_v<T>) {
            auto const underlying = static_cast<std::underlying_type_t<T>>(x);
            retval += detail::serialize_impl<Op, FieldOp>(
                underlying, field_number, os);
        } else if constexpr (std::is_integral_v<T>) {
            static_assert(
                sizeof(T) == 4u || (sizeof(T) == 8u && !std::is_signed_v<T>),
                "only 32-bit ints and unsigned ints, plus 64-bit unsigned "
                "ints, supported");
            if constexpr (FieldOp == ser_field_op::write)
                out = os::WriteVarint32ToArray(field_number, out);
            if constexpr (sizeof(T) == 8u)
                out = os::WriteVarint64ToArray(x, out);
            else if constexpr (std::is_signed_v<T>)
                out = os::WriteVarint32SignExtendedToArray(x, out);
            else
                out = os::WriteVarint32ToArray(x, out);
            detail::count_or_write<Op>(retval, buf, out - buf, os);
        } else if constexpr (std::is_floating_point_v<T>) {
            if constexpr (FieldOp == ser_field_op::write)
                out = os::WriteVarint32ToArray(field_number, out);
            if constexpr (std::is_same_v<T, float>) {
                out = os::WriteLittleEndian32ToArray(
                    std::bit_cast<uint32_t>(x), out);
                detail::count_or_write<Op>(retval, buf, out - buf, os);
            } else {
                static_assert(std::is_same_v<T, double>);
                out = os::WriteLittleEndian64ToArray(
                    std::bit_cast<uint64_t>(x), out);
                detail::count_or_write<Op>(retval, buf, out - buf, os);
            }
        } else if constexpr (std::is_same_v<T, std::string_view>) {
            if constexpr (FieldOp == ser_field_op::write)
                out = os::WriteVarint32ToArray(field_number, out);
            out = os::WriteVarint32ToArray(x.size(), out);
            detail::count_or_write<Op>(retval, buf, out - buf, os);
            detail::count_or_write<Op>(retval, x.data(), x.size(), os);
        } else if constexpr (std::is_same_v<T, std::string>) {
            std::string_view const sv(x);
            return detail::serialize_impl<Op, FieldOp>(sv, field_number, os);
        } else if constexpr (std::is_same_v<T, adobe::name_t>) {
            std::string_view const sv(x.c_str());
            return detail::serialize_impl<Op, FieldOp>(sv, field_number, os);
        } else if constexpr (is_vector_v<T>) {
            if constexpr (FieldOp == ser_field_op::write)
                out = os::WriteVarint32ToArray(field_number, out);
            out = os::WriteVarint32ToArray(x.size(), out);
            detail::count_or_write<Op>(retval, buf, out - buf, os);
            for (auto const & e : x) {
                retval += detail::serialize_impl<Op, ser_field_op::dont_write>(
                    e, 0, os);
            }
        } else if constexpr (is_map_v<T>) {
            if constexpr (FieldOp == ser_field_op::write)
                out = os::WriteVarint32ToArray(field_number, out);
            out = os::WriteVarint32ToArray(x.size(), out);
            detail::count_or_write<Op>(retval, buf, out - buf, os);
            for (auto const & [k, v] : x) {
                retval += detail::serialize_impl<Op, ser_field_op::dont_write>(
                    k, 0, os);
                retval += detail::serialize_impl<Op, ser_field_op::dont_write>(
                    v, 0, os);
            }
        } else {
            // generated for each Protobuf message type
            retval += serialize_message_impl<Op, FieldOp>(x, field_number, os);
        }

        return retval;
    }

    inline std::span<std::byte const>
    advance(std::span<std::byte const> src, std::ptrdiff_t size)
    {
        return src.last(src.size() - size);
    }

    constexpr int varint64_max_bytes = 10;
    constexpr int varint32_max_bytes = 5;

    inline std::span<std::byte const> read_varint_impl(
        uint32_t & x, uint32_t first_byte, std::span<std::byte const> src)
    {
        assert((uint32_t)src.front() == first_byte);

        int i = 0;

        x = first_byte - 0x80;
        ++i;

        uint32_t b = 0;
        b = (uint32_t)src[i++];
        x += b << 7;
        if (!(b & 0x80))
            return detail::advance(src, i);
        x -= 0x80 << 7;
        b = (uint32_t)src[i++];
        x += b << 14;
        if (!(b & 0x80))
            return detail::advance(src, i);
        x -= 0x80 << 14;
        b = (uint32_t)src[i++];
        x += b << 21;
        if (!(b & 0x80))
            return detail::advance(src, i);
        x -= 0x80 << 21;
        b = (uint32_t)src[i++];
        x += b << 28;
        if (!(b & 0x80))
            return detail::advance(src, i);

        throw failed_deserialization(
            "Encountered an int that is larger than 32 bits while trying to "
            "read a 32-bit int (probable corruption)");

        return {}; // unreachable
    }

    inline std::span<std::byte const>
    read_varint(uint32_t & x, std::span<std::byte const> src)
    {
        if (src.empty()) {
            throw failed_deserialization(
                "Encountered end of input while reading an integer.");
        }

        uint32_t const first_byte = (uint32_t)src.front();
        if (first_byte < 0x80u) {
            x = first_byte;
            return std::span(src.data() + 1, src.size() - 1);
        }

        if (src.size() < varint32_max_bytes &&
            (src.empty() || ((uint32_t)src.back() & 0x80) != 0)) {
            throw failed_deserialization(
                "Encountered too few bytes at end of input while trying to "
                "read an integer.");
        }

        return detail::read_varint_impl(x, first_byte, src);
    }

    template<int N>
    std::span<std::byte const>
    decode_varint_bytes(uint64_t & x, std::span<std::byte const> src)
    {
        static_assert(0 < N);
        x = static_cast<uint64_t>(src[N - 1]) << (7 * (N - 1));
        for (int i = 0, offset = 0; i < N - 1; i++, offset += 7) {
            x += (static_cast<uint64_t>(src[i]) - 0x80u) << offset;
        }
        return detail::advance(src, N);
    }

    inline std::span<std::byte const>
    read_varint_impl(uint64_t & x, std::span<std::byte const> src)
    {
        assert(0x80 <= (uint32_t)src[0]);

        if ((uint32_t)src[1] < 0x80)
            return detail::decode_varint_bytes<2>(x, src);
        else if ((uint32_t)src[2] < 0x80)
            return detail::decode_varint_bytes<3>(x, src);
        else if ((uint32_t)src[3] < 0x80)
            return detail::decode_varint_bytes<4>(x, src);
        else if ((uint32_t)src[4] < 0x80)
            return detail::decode_varint_bytes<5>(x, src);
        else if ((uint32_t)src[5] < 0x80)
            return detail::decode_varint_bytes<6>(x, src);
        else if ((uint32_t)src[6] < 0x80)
            return detail::decode_varint_bytes<7>(x, src);
        else if ((uint32_t)src[7] < 0x80)
            return detail::decode_varint_bytes<8>(x, src);
        else if ((uint32_t)src[8] < 0x80)
            return detail::decode_varint_bytes<9>(x, src);
        else if ((uint32_t)src[9] < 0x80)
            return detail::decode_varint_bytes<10>(x, src);

        throw failed_deserialization(
            "Encountered an int that is longer than 10 bytes (the max) "
            "while reading a 64-bit int (probable corruption)");

        return src; // unreachable
    }

    inline std::span<std::byte const>
    read_varint(uint64_t & x, std::span<std::byte const> src)
    {
        if (src.empty()) {
            throw failed_deserialization(
                "Encountered end of input while reading a 64-bit "
                "integer.");
        }

        uint32_t const first_byte = (uint32_t)src.front();
        if (first_byte < 0x80u) {
            x = first_byte;
            return std::span(src.data() + 1, src.size() - 1);
        }

        if (src.size() < varint64_max_bytes &&
            (src.empty() || ((uint32_t)src.back() & 0x80) != 0)) {
            throw failed_deserialization(
                "Encountered too few bytes at end of input while trying to "
                "read a 64-bit integer.");
        }

        return detail::read_varint_impl(x, src);
    }

    inline std::span<std::byte const>
    read_le(float & x, std::span<std::byte const> src)
    {
        uint32_t i = 0;
        auto in = reinterpret_cast<uint8_t const *>(src.data());
        in = is::ReadLittleEndian32FromArray(in, &i);
        x = std::bit_cast<float>(i);
        return detail::advance(
            src, reinterpret_cast<std::byte const *>(in) - src.data());
    }

    inline std::span<std::byte const>
    read_le(double & x, std::span<std::byte const> src)
    {
        uint64_t i = 0;
        auto in = reinterpret_cast<uint8_t const *>(src.data());
        in = is::ReadLittleEndian64FromArray(in, &i);
        x = std::bit_cast<double>(i);
        return detail::advance(
            src, reinterpret_cast<std::byte const *>(in) - src.data());
    }

    // a specialization is generated for each Protobuf message type
    template<typename T>
    std::span<std::byte const>
    deserialize_message_impl(T & x, std::span<std::byte const> src);

    template<typename T>
    std::span<std::byte const>
    deserialize_impl(T & x, std::span<std::byte const> src)
    {
        if constexpr (std::is_same_v<T, bool>) {
            if (src.empty()) {
                throw failed_deserialization(
                    "Encountered end of input while reading a bool.");
            }
            auto const b = (uint32_t)src.front();
            if (b != 1 && b != 0) {
                throw failed_deserialization(std::format(
                    "Encountered unexpected value{} reading a bool.", b));
            }
            x = (bool)b;
            return src.last(src.size() - 1);
        } else if constexpr (std::is_enum_v<T>) {
            std::underlying_type_t<T> underlying;
            src = detail::deserialize_impl(underlying, src);
            x = static_cast<T>(underlying);
            return src;
        } else if constexpr (std::is_integral_v<T>) {
            static_assert(
                sizeof(T) == 4u || (sizeof(T) == 8u && !std::is_signed_v<T>),
                "only 32-bit ints and unsigned ints, plus 64-bit unsigned "
                "ints, supported");
            if constexpr (std::is_signed_v<T> || sizeof(T) == 8u) {
                uint64_t temp;
                src = detail::read_varint(temp, src);
                x = static_cast<T>(temp);
                return src;
            } else {
                return detail::read_varint(x, src);
            }
        } else if constexpr (std::is_floating_point_v<T>) {
            static_assert(
                std::is_same_v<T, float> || std::is_same_v<T, double>,
                "only floats and doubles supported.");
            return detail::read_le(x, src);
        } else if constexpr (std::is_same_v<T, std::string>) {
            uint32_t size = 0;
            src = detail::read_varint(size, src);
            x.resize(size);
            std::memcpy(x.data(), src.data(), size);
            return detail::advance(src, size);
        } else if constexpr (std::is_same_v<T, adobe::name_t>) {
            uint32_t size = 0;
            src = detail::read_varint(size, src);
            std::string str;
            str.resize(size);
            std::memcpy(str.data(), src.data(), size);
            x = adobe::name_t(str.c_str());
            return detail::advance(src, size);
        } else if constexpr (is_vector_v<T>) {
            // TODO: Add try/catch, and add info on which element failed (here
            // and for map).
            uint32_t size = 0;
            src = detail::read_varint(size, src);
            x.reserve(size);
            typename T::value_type e;
            for (int i = 0; i < (int)size; ++i) {
                src = detail::deserialize_impl(e, src);
                x.push_back(std::move(e));
            }
            return src;
        } else if constexpr (is_map_v<T>) {
            uint32_t size = 0;
            src = detail::read_varint(size, src);
            if constexpr (requires(T x) { x.reserve(size); })
                x.reserve(size);
            typename T::key_type k;
            typename T::mapped_type v;
            for (int i = 0; i < (int)size; ++i) {
                src = detail::deserialize_impl(k, src);
                src = detail::deserialize_impl(v, src);
                x.emplace(std::move(k), std::move(v));
            }
            return src;
        } else {
            // generated for each Protobuf message type
            return detail::deserialize_message_impl(x, src);
        }
    }

    // These two should only be called from within generated overloads of
    // serialize_message_impl().
    template<ser_op Op>
    std::ptrdiff_t serialize_message_end(std::ostream * os)
    {
        std::ptrdiff_t retval = 0;
        uint8_t buf[8];
        uint8_t * out = buf;
        out = os::WriteVarint32ToArray(0, out);
        detail::count_or_write<Op>(retval, buf, out - buf, os);
        return retval;
    }

    template<int LoFieldNumber, int HiFieldNumber, typename T, typename F>
    std::span<std::byte const> deserialize_message_impl_impl(
        T & x,
        std::span<std::byte const> src,
        std::string_view this_message_name,
        std::array<std::string_view, HiFieldNumber + 1> const & field_names,
        std::array<int, HiFieldNumber> & expected_field_numbers,
        F && read_field)
    {
        std::span<int, std::dynamic_extent> field_numbers_remaining(
            expected_field_numbers);

        bool read_end_field_number = false;
        do {
            uint32_t field_number = -1;
            try {
                src = detail::read_varint(field_number, src);
            } catch (failed_deserialization & e) {
                e.add_info(std::format(
                    "While reading the next field of {}:", this_message_name));
                throw e;
            }

            if (field_number == 0) {
                std::wstring wthis_message_name(
                    this_message_name.begin(), this_message_name.end());
                std::wstring msg = std::format(
                    L"Encountered unexpected end of messagebefore reading all "
                    L"the expected fields while reading a {}.",
                    wthis_message_name);
                UE_LOG(general, Log, TEXT("{}"), *FSTRINGIZE(msg));
                read_end_field_number = true;
                break;
            }

            if (field_number < LoFieldNumber || HiFieldNumber < field_number) {
                throw failed_deserialization(std::format(
                    "Encountered unexpected field number {} while reading a {} "
                    "(first field number is {}; last field number is {}).",
                    field_number,
                    this_message_name,
                    LoFieldNumber,
                    HiFieldNumber));
            }

            {
                auto const it =
                    std::ranges::find(field_numbers_remaining, field_number);
                if (it == field_numbers_remaining.end()) {
                    throw failed_deserialization(std::format(
                        "Encountered duplicate field number {} ({}) while "
                        "reading a {}.",
                        field_number,
                        field_names[field_number],
                        this_message_name));
                }
                std::swap(*it, field_numbers_remaining.back());
                field_numbers_remaining = field_numbers_remaining.first(
                    field_numbers_remaining.size() - 1u);
            }

            try {
                src = read_field(x, field_number, src);
            } catch (failed_deserialization & e) {
                e.add_info(std::format(
                    "While reading {}::{}:",
                    this_message_name,
                    field_names[field_number]));
                throw e;
            }
        } while (!field_numbers_remaining.empty());

        if (!read_end_field_number) {
            uint32_t field_number = -1;
            src = detail::read_varint(field_number, src);
            if (field_number != 0) {
                throw failed_deserialization(std::format(
                    "Encountered unexpected field number {} while reading a {} "
                    "(field number 0/end of message expected).",
                    field_number,
                    this_message_name));
            }
        }

        return src;
    }
}

template<typename T>
std::ptrdiff_t serialized_size(T const & x)
{
    return detail::
        serialize_impl<detail::ser_op::count, detail::ser_field_op::dont_write>(
            x, 0, nullptr);
}

template<typename T>
void serialize_message(T const & x, std::filesystem::path const & path)
{
    std::ofstream ofs(path, std::ios::binary);
    if (!ofs) {
        throw std::runtime_error(std::format(
            "serialize_message(): Path {} exists, but could not be opened for "
            "writing.",
            path.generic_string()));
    }
    detail::
        serialize_impl<detail::ser_op::write, detail::ser_field_op::dont_write>(
            x, 0, &ofs);
}

template<typename T>
void deserialize_message(T & x, std::filesystem::path const & path)
{
    if (!std::filesystem::exists(path)) {
        throw failed_deserialization(std::format(
            "deserialize_message(): Path {} does not exist.",
            path.generic_string()));
    }
    try {
        memmap mm(path);
        detail::deserialize_impl(x, mm.bytes());
    } catch (failed_memmap const & e) {
        throw failed_deserialization(std::format(
            "deserialize_message(): Path {} exists, but could not be memmapped "
            "for reading. ({})",
            path.generic_string(),
            e.what()));
    }
}
