#include "serialize_impl_test_messages.hpp"

#include <model.hpp>
#include <json2pb.h>
#include <generate_galaxy.hpp>
#include <serialization.hpp>

#include <gtest/gtest.h>

#include <format>
#include <strstream>


template<std::ranges::range R>
auto byte_span_of(R const & r)
{
    return std::as_bytes(std::span(r.begin(), r.end()));
}

enum struct enumu32 : uint32_t { value_1, value_2 = 999, value_3 = 999999 };
enum struct enumi32 : int32_t { value_1, value_2 = -999, value_3 = -999999 };

TEST(serialization_tests, serialize_impl_enums_round_trips)
{
    using namespace detail;

    // with field
    {
        std::ostringstream oss(std::ios::binary);
        auto x = enumu32::value_1;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = enumu32::value_1;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 700);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = enumu32::value_2;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = enumu32::value_2;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 700);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = enumu32::value_3;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = enumu32::value_3;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 700);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }

    {
        std::ostringstream oss(std::ios::binary);
        auto x = enumi32::value_1;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = enumi32::value_1;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 700);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = enumi32::value_2;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = enumi32::value_2;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 700);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = enumi32::value_3;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = enumi32::value_3;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 700);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }

    // without field
    {
        std::ostringstream oss(std::ios::binary);
        auto x = enumu32::value_1;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = enumu32::value_1;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = enumu32::value_2;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = enumu32::value_2;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = enumu32::value_3;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = enumu32::value_3;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }

    {
        std::ostringstream oss(std::ios::binary);
        auto x = enumi32::value_1;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = enumi32::value_1;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = enumi32::value_2;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = enumi32::value_2;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = enumi32::value_3;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = enumi32::value_3;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
}

TEST(serialization_tests, serialize_impl_ints_round_trips)
{
    using namespace detail;

    // with field
    {
        std::ostringstream oss(std::ios::binary);
        auto x = 0;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 1, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 1);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = 0;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = 1;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = -1;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        unsigned int x = 4'000'000'000;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 700);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        int x = -4'000'000'000;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 700);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }

    // without field
    {
        std::ostringstream oss(std::ios::binary);
        auto x = 0;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 1, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = 0;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = 1;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = -1;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        unsigned int x = 4'000'000'000;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        int x = -4'000'000'000;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
}

TEST(serialization_tests, serialize_impl_float_round_trips)
{
    using namespace detail;

    // with field
    {
        std::ostringstream oss(std::ios::binary);
        auto x = 0.f;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 1, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 1);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = 0.f;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = 1.f;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = -1.f;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = 10000.f;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = -10000.f;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }

    // without field
    {
        std::ostringstream oss(std::ios::binary);
        auto x = 0.f;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 1, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = 0.f;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = 1.f;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = -1.f;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = 10000.f;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = -10000.f;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
}

TEST(serialization_tests, serialize_impl_double_round_trips)
{
    using namespace detail;

    // with field
    {
        std::ostringstream oss(std::ios::binary);
        auto x = 0.0;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 1, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 1);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = 0.0;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = 1.0;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = -1.0;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = 10000.0;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = -10000.0;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }

    // without field
    {
        std::ostringstream oss(std::ios::binary);
        auto x = 0.0;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 1, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = 0.0;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = 1.0;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = -1.0;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = 10000.0;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = -10000.0;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
}

TEST(serialization_tests, serialize_impl_string_round_trips)
{
    using namespace detail;
    using namespace std::literals;

    // with field
    {
        std::ostringstream oss(std::ios::binary);
        auto x = ""s;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 1, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 1);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = ""s;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = ""s;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 700);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = "some text"s;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = "some text"s;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 700);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }

    // without field
    {
        std::ostringstream oss(std::ios::binary);
        auto x = ""s;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 1, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = ""s;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = ""s;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = "some text"s;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = "some text"s;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy = {};
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
}

TEST(serialization_tests, serialize_impl_name_round_trips)
{
    using namespace detail;
    using namespace std::literals;

    // with field
    {
        std::ostringstream oss(std::ios::binary);
        auto x = ""s;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 1, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 1);
        adobe::name_t x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, adobe::name_t(x.c_str()));
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = ""s;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        adobe::name_t x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, adobe::name_t(x.c_str()));
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = ""s;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 700);
        adobe::name_t x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, adobe::name_t(x.c_str()));
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = "some text"s;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        adobe::name_t x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, adobe::name_t(x.c_str()));
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = "some text"s;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 700);
        adobe::name_t x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, adobe::name_t(x.c_str()));
        EXPECT_TRUE(src.empty());
    }

    // without field
    {
        std::ostringstream oss(std::ios::binary);
        auto x = ""s;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 1, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        adobe::name_t x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, adobe::name_t(x.c_str()));
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = ""s;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        adobe::name_t x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, adobe::name_t(x.c_str()));
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = ""s;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        adobe::name_t x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, adobe::name_t(x.c_str()));
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = "some text"s;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        adobe::name_t x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, adobe::name_t(x.c_str()));
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto x = "some text"s;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        adobe::name_t x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, adobe::name_t(x.c_str()));
        EXPECT_TRUE(src.empty());
    }
}

TEST(serialization_tests, serialize_impl_vec_round_trips)
{
    using namespace detail;
    using namespace std::literals;

    // with field
    // int
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<int> x;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 1, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 1);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<int> x;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<int> x;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 700);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<int> x({1, 2, 3, 4, 5});
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<int> x({1, 2, 3, 4, 5});
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 700);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    // double
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<double> x;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 1, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 1);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<double> x;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<double> x;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 700);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<double> x({1.0, 2.0, 3.0, 4.0, 5.0});
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<double> x({1.0, 2.0, 3.0, 4.0, 5.0});
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 700);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    // vector<int>
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<std::vector<int>> x;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 1, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 1);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<std::vector<int>> x;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<std::vector<int>> x;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 700);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<std::vector<int>> x(
            {std::vector<int>{},
             std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>({1, 2, 3, 4, 5})});
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<std::vector<int>> x(
            {std::vector<int>{},
             std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>({1, 2, 3, 4, 5})});
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 700);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<std::vector<int>> x(
            {std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>{},
             std::vector<int>({1, 2, 3, 4, 5})});
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<std::vector<int>> x(
            {std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>{},
             std::vector<int>({1, 2, 3, 4, 5})});
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 700);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<std::vector<int>> x(
            {std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>{}});
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<std::vector<int>> x(
            {std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>{}});
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 700);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<std::vector<int>> x(
            {std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>({1, 2, 3, 4, 5})});
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<std::vector<int>> x(
            {std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>({1, 2, 3, 4, 5})});
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 700);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }

    // without field
    // int
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<int> x;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 1, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<int> x;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<int> x;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<int> x({1, 2, 3, 4, 5});
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<int> x({1, 2, 3, 4, 5});
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    // double
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<double> x;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 1, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<double> x;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<double> x;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<double> x({1.0, 2.0, 3.0, 4.0, 5.0});
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<double> x({1.0, 2.0, 3.0, 4.0, 5.0});
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    // vector<int>
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<std::vector<int>> x;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 1, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<std::vector<int>> x;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<std::vector<int>> x;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<std::vector<int>> x(
            {std::vector<int>{},
             std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>({1, 2, 3, 4, 5})});
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<std::vector<int>> x(
            {std::vector<int>{},
             std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>({1, 2, 3, 4, 5})});
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<std::vector<int>> x(
            {std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>{},
             std::vector<int>({1, 2, 3, 4, 5})});
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<std::vector<int>> x(
            {std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>{},
             std::vector<int>({1, 2, 3, 4, 5})});
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<std::vector<int>> x(
            {std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>{}});
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<std::vector<int>> x(
            {std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>{}});
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<std::vector<int>> x(
            {std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>({1, 2, 3, 4, 5})});
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        std::vector<std::vector<int>> x(
            {std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>({1, 2, 3, 4, 5})});
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
}

TEST(serialization_tests, serialize_impl_map_round_trips)
{
    using namespace detail;
    using namespace std::literals;

    // with field
    // int
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, int> x;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 1, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 1);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, int> x;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, int> x;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 700);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, int> x({
            std::pair{"one"s, 1},
            std::pair{"two"s, 2},
            std::pair{"three"s, 3},
            std::pair{"four"s, 4},
            std::pair{"five"s, 5},
        });
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, int> x({
            std::pair{"one"s, 1},
            std::pair{"two"s, 2},
            std::pair{"three"s, 3},
            std::pair{"four"s, 4},
            std::pair{"five"s, 5},
        });
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 700);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    // double
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, double> x;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 1, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 1);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, double> x;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, double> x;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 700);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, double> x({
            std::pair{"one"s, 1.0},
            std::pair{"two"s, 2.0},
            std::pair{"three"s, 3.0},
            std::pair{"four"s, 4.0},
            std::pair{"five"s, 5.0},
        });
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, double> x({
            std::pair{"one"s, 1.0},
            std::pair{"two"s, 2.0},
            std::pair{"three"s, 3.0},
            std::pair{"four"s, 4.0},
            std::pair{"five"s, 5.0},
        });
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 700);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    // vector<int>
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, std::vector<int>> x;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 1, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 1);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, std::vector<int>> x;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, std::vector<int>> x;
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 700);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, std::vector<int>> x(
            {std::pair{"a"s, std::vector<int>{}},
             std::pair{"b"s, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"c"s, std::vector<int>({1, 2, 3, 4, 5})}});
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, std::vector<int>> x(
            {std::pair{"a"s, std::vector<int>{}},
             std::pair{"b"s, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"c"s, std::vector<int>({1, 2, 3, 4, 5})}});
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 700);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, std::vector<int>> x(
            {std::pair{"a"s, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"b"s, std::vector<int>{}},
             std::pair{"c"s, std::vector<int>({1, 2, 3, 4, 5})}});
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, std::vector<int>> x(
            {std::pair{"a"s, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"b"s, std::vector<int>{}},
             std::pair{"c"s, std::vector<int>({1, 2, 3, 4, 5})}});
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 700);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, std::vector<int>> x(
            {std::pair{"a"s, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"b"s, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"c"s, std::vector<int>{}}});
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, std::vector<int>> x(
            {std::pair{"a"s, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"b"s, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"c"s, std::vector<int>{}}});
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 700);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, std::vector<int>> x(
            {std::pair{"a"s, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"b"s, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"c"s, std::vector<int>({1, 2, 3, 4, 5})}});
        serialize_impl<ser_op::write, ser_field_op::write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 7);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, std::vector<int>> x(
            {std::pair{"a"s, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"b"s, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"c"s, std::vector<int>({1, 2, 3, 4, 5})}});
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 700);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }

    // without field
    // int
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, int> x;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 1, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, int> x;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, int> x;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, int> x({
            std::pair{"one"s, 1},
            std::pair{"two"s, 2},
            std::pair{"three"s, 3},
            std::pair{"four"s, 4},
            std::pair{"five"s, 5},
        });
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, int> x({
            std::pair{"one"s, 1},
            std::pair{"two"s, 2},
            std::pair{"three"s, 3},
            std::pair{"four"s, 4},
            std::pair{"five"s, 5},
        });
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    // double
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, double> x;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 1, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, double> x;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, double> x;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, double> x({
            std::pair{"one"s, 1.0},
            std::pair{"two"s, 2.0},
            std::pair{"three"s, 3.0},
            std::pair{"four"s, 4.0},
            std::pair{"five"s, 5.0},
        });
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, double> x({
            std::pair{"one"s, 1.0},
            std::pair{"two"s, 2.0},
            std::pair{"three"s, 3.0},
            std::pair{"four"s, 4.0},
            std::pair{"five"s, 5.0},
        });
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    // vector<int>
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, std::vector<int>> x;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 1, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, std::vector<int>> x;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, std::vector<int>> x;
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, std::vector<int>> x(
            {std::pair{"a"s, std::vector<int>{}},
             std::pair{"b"s, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"c"s, std::vector<int>({1, 2, 3, 4, 5})}});
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, std::vector<int>> x(
            {std::pair{"a"s, std::vector<int>{}},
             std::pair{"b"s, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"c"s, std::vector<int>({1, 2, 3, 4, 5})}});
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, std::vector<int>> x(
            {std::pair{"a"s, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"b"s, std::vector<int>{}},
             std::pair{"c"s, std::vector<int>({1, 2, 3, 4, 5})}});
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, std::vector<int>> x(
            {std::pair{"a"s, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"b"s, std::vector<int>{}},
             std::pair{"c"s, std::vector<int>({1, 2, 3, 4, 5})}});
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, std::vector<int>> x(
            {std::pair{"a"s, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"b"s, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"c"s, std::vector<int>{}}});
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, std::vector<int>> x(
            {std::pair{"a"s, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"b"s, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"c"s, std::vector<int>{}}});
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, std::vector<int>> x(
            {std::pair{"a"s, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"b"s, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"c"s, std::vector<int>({1, 2, 3, 4, 5})}});
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 7, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        boost::container::flat_map<std::string, std::vector<int>> x(
            {std::pair{"a"s, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"b"s, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"c"s, std::vector<int>({1, 2, 3, 4, 5})}});
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
}

TEST(serialization_tests, serialize_impl_nonnested_message_round_trips)
{
    using namespace detail;
    using namespace std::literals;

    // with field
    {
        std::ostringstream oss(std::ios::binary);
        detail::by_hand::unit_t x = {
            .design_id = 142, .design_owner = 800, .health = 0};
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 700);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        detail::by_hand::unit_t x = {
            .design_id = 142, .design_owner = 0, .health = 190};
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 700);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        detail::by_hand::unit_t x = {
            .design_id = 0, .design_owner = 800, .health = 190};
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 700);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }

    // without field
    {
        std::ostringstream oss(std::ios::binary);
        detail::by_hand::unit_t x = {
            .design_id = 142, .design_owner = 800, .health = 0};
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        detail::by_hand::unit_t x = {
            .design_id = 142, .design_owner = 0, .health = 190};
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        detail::by_hand::unit_t x = {
            .design_id = 0, .design_owner = 800, .health = 190};
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
}

TEST(serialization_tests, serialize_impl_nested_message_round_trips)
{
    using namespace detail;
    using namespace std::literals;

    detail::by_hand::unit_t u1 = {
        .design_id = 142, .design_owner = 800, .health = 0};
    detail::by_hand::unit_t u2 = {
        .design_id = 142, .design_owner = 0, .health = 190};
    detail::by_hand::unit_t u3 = {
        .design_id = 0, .design_owner = 800, .health = 190};

    // with field
    {
        std::ostringstream oss(std::ios::binary);
        detail::by_hand::fleet_t x = {
            .id = 189,
            .mission = detail::by_hand::mission_t::expand_colony,
            .units = {u1, u2, u3},
            .fuel = 27e6,
            .rounds = 1337,
            .missiles = 42,
            .fighters = 13};
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 700);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        detail::by_hand::fleet_t x = {
            .id = 189,
            .mission = detail::by_hand::mission_t::expand_colony,
            .units = {u1, u3, u2},
            .fuel = 27e6,
            .rounds = 1337,
            .missiles = 42,
            .fighters = 13};
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 700);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        detail::by_hand::fleet_t x = {
            .id = 189,
            .mission = detail::by_hand::mission_t::expand_colony,
            .units = {u2, u3, u1},
            .fuel = 27e6,
            .rounds = 1337,
            .missiles = 42,
            .fighters = 13};
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        uint32_t field_number = 0;
        src = detail::deserialize_impl(field_number, src);
        EXPECT_EQ(field_number, 700);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }

    // without field
    {
        std::ostringstream oss(std::ios::binary);
        detail::by_hand::fleet_t x = {
            .id = 189,
            .mission = detail::by_hand::mission_t::expand_colony,
            .units = {u1, u2, u3},
            .fuel = 27e6,
            .rounds = 1337,
            .missiles = 42,
            .fighters = 13};
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        detail::by_hand::fleet_t x = {
            .id = 189,
            .mission = detail::by_hand::mission_t::expand_colony,
            .units = {u1, u3, u2},
            .fuel = 27e6,
            .rounds = 1337,
            .missiles = 42,
            .fighters = 13};
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
    {
        std::ostringstream oss(std::ios::binary);
        detail::by_hand::fleet_t x = {
            .id = 189,
            .mission = detail::by_hand::mission_t::expand_colony,
            .units = {u2, u3, u1},
            .fuel = 27e6,
            .rounds = 1337,
            .missiles = 42,
            .fighters = 13};
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const output = oss.str();
        auto src = byte_span_of(output);
        decltype(x) x_copy;
        src = detail::deserialize_impl(x_copy, src);
        EXPECT_EQ(x_copy, x);
        EXPECT_TRUE(src.empty());
    }
}
