#include "serialize_impl_test_messages.hpp"

#include <model.hpp>
#include <json2pb.h>
#include <generate_galaxy.hpp>
#include <serialization.hpp>

#include <gtest/gtest.h>

#include <format>
#include <strstream>


TEST(serialization_tests, memmap)
{
    std::filesystem::path path("memmap_file");
    std::filesystem::remove(path);

    std::string const contents = "1 2 3 4 5 6 7 8 9 10";

    std::ofstream ofs(path, std::ios::binary);
    auto const initial_tellp = ofs.tellp();
    ofs << contents;
    auto const bytes_written = ofs.tellp() - initial_tellp;
    ofs.close();

    memmap mm(path);

    auto const memmapped_bytes = mm.bytes();
    EXPECT_EQ(memmapped_bytes.size(), bytes_written);
    EXPECT_TRUE(
        std::ranges::equal(memmapped_bytes, contents, [](auto a, auto b) {
            return (unsigned char)a == (unsigned char)b;
        }));
}

TEST(serialization_tests, memmap_nonexistent_file)
{
    std::filesystem::path path("nonexistent_file_unlikely_name_123456789");
    std::filesystem::remove(path);

    try {
        memmap mm(path);
        EXPECT_FALSE("The line above should have thrown.");
    } catch (failed_memmap const & e) {
    } catch (...) {
        EXPECT_FALSE(
            "The line above should have thrown a failed_memmap exception.");
    }
}

enum struct enumu32 : uint32_t { value_1, value_2, value_3 = 999 };
enum struct enumi32 : int32_t { value_1, value_2, value_3 = -999 };

TEST(serialization_tests, serialize_impl_enums_counts)
{
    using namespace detail;

    // with field
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(
            enumu32::value_1, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                enumu32::value_1, 7, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(
            enumu32::value_1, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                enumu32::value_1, 700, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(
            enumu32::value_2, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                enumu32::value_2, 7, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(
            enumu32::value_2, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                enumu32::value_2, 700, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(
            enumu32::value_3, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                enumu32::value_3, 7, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(
            enumu32::value_3, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                enumu32::value_3, 700, nullptr)));
    }

    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(
            enumi32::value_1, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                enumi32::value_1, 7, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(
            enumi32::value_1, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                enumi32::value_1, 700, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(
            enumi32::value_2, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                enumi32::value_2, 7, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(
            enumi32::value_2, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                enumi32::value_2, 700, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(
            enumi32::value_3, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                enumi32::value_3, 7, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(
            enumi32::value_3, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                enumi32::value_3, 700, nullptr)));
    }

    // without field
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(
            enumu32::value_1, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                enumu32::value_1, 7, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(
            enumu32::value_1, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                enumu32::value_1, 700, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(
            enumu32::value_2, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                enumu32::value_2, 7, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(
            enumu32::value_2, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                enumu32::value_2, 700, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(
            enumu32::value_3, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                enumu32::value_3, 7, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(
            enumu32::value_3, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                enumu32::value_3, 700, nullptr)));
    }

    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(
            enumi32::value_1, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                enumi32::value_1, 7, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(
            enumi32::value_1, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                enumi32::value_1, 700, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(
            enumi32::value_2, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                enumi32::value_2, 7, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(
            enumi32::value_2, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                enumi32::value_2, 700, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(
            enumi32::value_3, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                enumi32::value_3, 7, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(
            enumi32::value_3, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                enumi32::value_3, 700, nullptr)));
    }
}

TEST(serialization_tests, serialize_impl_ints_counts)
{
    using namespace detail;

    // with field
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(0, 1, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                0, 1, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(0, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                0, 7, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(1, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                1, 7, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(-1, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                -1, 7, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(10000, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                10000, 700, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(-10000, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                -10000, 700, nullptr)));
    }

    // without field
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(0, 1, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                0, 1, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(0, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                0, 7, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(1, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                1, 7, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(-1, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                -1, 7, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(
            10000, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                10000, 700, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(
            -10000, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                -10000, 700, nullptr)));
    }
}

TEST(serialization_tests, serialize_impl_float_counts)
{
    using namespace detail;

    // with field
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(0.f, 1, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                0.f, 1, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(0.f, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                0.f, 7, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(1.f, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                1.f, 7, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(-1.f, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                -1.f, 7, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(10000.f, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                10000.f, 700, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(-10000.f, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                -10000.f, 700, nullptr)));
    }

    // without field
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(0.f, 1, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                0.f, 1, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(0.f, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                0.f, 7, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(1.f, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                1.f, 7, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(-1.f, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                -1.f, 7, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(
            10000.f, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                10000.f, 700, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(
            -10000.f, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                -10000.f, 700, nullptr)));
    }
}

TEST(serialization_tests, serialize_impl_double_counts)
{
    using namespace detail;

    // with field
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(0.0, 1, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                0.0, 1, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(0.0, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                0.0, 7, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(1.0, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                1.0, 7, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(-1.0, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                -1.0, 7, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(10000.0, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                10000.0, 700, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(-10000.0, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                -10000.0, 700, nullptr)));
    }

    // without field
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(0.0, 1, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                0.0, 1, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(0.0, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                0.0, 7, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(1.0, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                1.0, 7, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(-1.0, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                -1.0, 7, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(
            10000.0, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                10000.0, 700, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(
            -10000.0, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                -10000.0, 700, nullptr)));
    }
}

TEST(serialization_tests, serialize_impl_string_counts)
{
    using namespace detail;
    using namespace std::literals;

    // with field
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(""sv, 1, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                ""sv, 1, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(""sv, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                ""sv, 7, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(""sv, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                ""sv, 700, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(
            "some text"sv, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                "some text"sv, 7, nullptr)));
    }
    {
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(
            "some text"sv, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                "some text"sv, 700, nullptr)));
    }
}

TEST(serialization_tests, serialize_impl_vec_counts)
{
    using namespace detail;
    using namespace std::literals;

    // with field
    // int
    {
        std::vector<int> v;
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(v, 1, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                v, 1, nullptr)));
    }
    {
        std::vector<int> v;
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(v, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                v, 7, nullptr)));
    }
    {
        std::vector<int> v;
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(v, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                v, 700, nullptr)));
    }
    {
        std::vector<int> v({1, 2, 3, 4, 5});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(v, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                v, 7, nullptr)));
    }
    {
        std::vector<int> v({1, 2, 3, 4, 5});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(v, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                v, 700, nullptr)));
    }
    // double
    {
        std::vector<double> v;
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(v, 1, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                v, 1, nullptr)));
    }
    {
        std::vector<double> v;
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(v, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                v, 7, nullptr)));
    }
    {
        std::vector<double> v;
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(v, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                v, 700, nullptr)));
    }
    {
        std::vector<double> v({1.0, 2.0, 3.0, 4.0, 5.0});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(v, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                v, 7, nullptr)));
    }
    {
        std::vector<double> v({1.0, 2.0, 3.0, 4.0, 5.0});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(v, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                v, 700, nullptr)));
    }
    // vector<int>
    {
        std::vector<std::vector<int>> v;
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(v, 1, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                v, 1, nullptr)));
    }
    {
        std::vector<std::vector<int>> v;
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(v, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                v, 7, nullptr)));
    }
    {
        std::vector<std::vector<int>> v;
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(v, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                v, 700, nullptr)));
    }
    {
        std::vector<std::vector<int>> v(
            {std::vector<int>{},
             std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>({1, 2, 3, 4, 5})});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(v, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                v, 7, nullptr)));
    }
    {
        std::vector<std::vector<int>> v(
            {std::vector<int>{},
             std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>({1, 2, 3, 4, 5})});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(v, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                v, 700, nullptr)));
    }
    {
        std::vector<std::vector<int>> v(
            {std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>{},
             std::vector<int>({1, 2, 3, 4, 5})});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(v, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                v, 7, nullptr)));
    }
    {
        std::vector<std::vector<int>> v(
            {std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>{},
             std::vector<int>({1, 2, 3, 4, 5})});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(v, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                v, 700, nullptr)));
    }
    {
        std::vector<std::vector<int>> v(
            {std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>{}});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(v, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                v, 7, nullptr)));
    }
    {
        std::vector<std::vector<int>> v(
            {std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>{}});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(v, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                v, 700, nullptr)));
    }
    {
        std::vector<std::vector<int>> v(
            {std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>({1, 2, 3, 4, 5})});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(v, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                v, 7, nullptr)));
    }
    {
        std::vector<std::vector<int>> v(
            {std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>({1, 2, 3, 4, 5})});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(v, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                v, 700, nullptr)));
    }

    // without field
    // int
    {
        std::vector<int> v;
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(v, 1, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                v, 1, nullptr)));
    }
    {
        std::vector<int> v;
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(v, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                v, 7, nullptr)));
    }
    {
        std::vector<int> v;
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(v, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                v, 700, nullptr)));
    }
    {
        std::vector<int> v({1, 2, 3, 4, 5});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(v, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                v, 7, nullptr)));
    }
    {
        std::vector<int> v({1, 2, 3, 4, 5});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(v, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                v, 700, nullptr)));
    }
    // double
    {
        std::vector<double> v;
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(v, 1, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                v, 1, nullptr)));
    }
    {
        std::vector<double> v;
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(v, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                v, 7, nullptr)));
    }
    {
        std::vector<double> v;
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(v, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                v, 700, nullptr)));
    }
    {
        std::vector<double> v({1.0, 2.0, 3.0, 4.0, 5.0});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(v, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                v, 7, nullptr)));
    }
    {
        std::vector<double> v({1.0, 2.0, 3.0, 4.0, 5.0});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(v, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                v, 700, nullptr)));
    }
    // vector<int>
    {
        std::vector<std::vector<int>> v;
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(v, 1, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                v, 1, nullptr)));
    }
    {
        std::vector<std::vector<int>> v;
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(v, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                v, 7, nullptr)));
    }
    {
        std::vector<std::vector<int>> v;
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(v, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                v, 700, nullptr)));
    }
    {
        std::vector<std::vector<int>> v(
            {std::vector<int>{},
             std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>({1, 2, 3, 4, 5})});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(v, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                v, 7, nullptr)));
    }
    {
        std::vector<std::vector<int>> v(
            {std::vector<int>{},
             std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>({1, 2, 3, 4, 5})});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(v, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                v, 700, nullptr)));
    }
    {
        std::vector<std::vector<int>> v(
            {std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>{},
             std::vector<int>({1, 2, 3, 4, 5})});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(v, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                v, 7, nullptr)));
    }
    {
        std::vector<std::vector<int>> v(
            {std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>{},
             std::vector<int>({1, 2, 3, 4, 5})});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(v, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                v, 700, nullptr)));
    }
    {
        std::vector<std::vector<int>> v(
            {std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>{}});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(v, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                v, 7, nullptr)));
    }
    {
        std::vector<std::vector<int>> v(
            {std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>{}});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(v, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                v, 700, nullptr)));
    }
    {
        std::vector<std::vector<int>> v(
            {std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>({1, 2, 3, 4, 5})});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(v, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                v, 7, nullptr)));
    }
    {
        std::vector<std::vector<int>> v(
            {std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>({1, 2, 3, 4, 5}),
             std::vector<int>({1, 2, 3, 4, 5})});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(v, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                v, 700, nullptr)));
    }
}

TEST(serialization_tests, serialize_impl_map_counts)
{
    using namespace detail;
    using namespace std::literals;

    // with field
    // int
    {
        boost::container::flat_map<std::string_view, int> m;
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(m, 1, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                m, 1, nullptr)));
    }
    {
        boost::container::flat_map<std::string_view, int> m;
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(m, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                m, 7, nullptr)));
    }
    {
        boost::container::flat_map<std::string_view, int> m;
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(m, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                m, 700, nullptr)));
    }
    {
        boost::container::flat_map<std::string_view, int> m({
            std::pair{"one"sv, 1},
            std::pair{"two"sv, 2},
            std::pair{"three"sv, 3},
            std::pair{"four"sv, 4},
            std::pair{"five"sv, 5},
        });
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(m, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                m, 7, nullptr)));
    }
    {
        boost::container::flat_map<std::string_view, int> m({
            std::pair{"one"sv, 1},
            std::pair{"two"sv, 2},
            std::pair{"three"sv, 3},
            std::pair{"four"sv, 4},
            std::pair{"five"sv, 5},
        });
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(m, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                m, 700, nullptr)));
    }
    // double
    {
        boost::container::flat_map<std::string_view, double> m;
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(m, 1, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                m, 1, nullptr)));
    }
    {
        boost::container::flat_map<std::string_view, double> m;
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(m, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                m, 7, nullptr)));
    }
    {
        boost::container::flat_map<std::string_view, double> m;
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(m, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                m, 700, nullptr)));
    }
    {
        boost::container::flat_map<std::string_view, int> m({
            std::pair{"one"sv, 1.0},
            std::pair{"two"sv, 2.0},
            std::pair{"three"sv, 3.0},
            std::pair{"four"sv, 4.0},
            std::pair{"five"sv, 5.0},
        });
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(m, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                m, 7, nullptr)));
    }
    {
        boost::container::flat_map<std::string_view, int> m({
            std::pair{"one"sv, 1.0},
            std::pair{"two"sv, 2.0},
            std::pair{"three"sv, 3.0},
            std::pair{"four"sv, 4.0},
            std::pair{"five"sv, 5.0},
        });
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(m, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                m, 700, nullptr)));
    }
    // vector<int>
    {
        boost::container::flat_map<std::string_view, std::vector<int>> m;
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(m, 1, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                m, 1, nullptr)));
    }
    {
        boost::container::flat_map<std::string_view, std::vector<int>> m;
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(m, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                m, 7, nullptr)));
    }
    {
        boost::container::flat_map<std::string_view, std::vector<int>> m;
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(m, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                m, 700, nullptr)));
    }
    {
        boost::container::flat_map<std::string_view, std::vector<int>> m(
            {std::pair{"a"sv, std::vector<int>{}},
             std::pair{"b"sv, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"c"sv, std::vector<int>({1, 2, 3, 4, 5})}});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(m, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                m, 7, nullptr)));
    }
    {
        boost::container::flat_map<std::string_view, std::vector<int>> m(
            {std::pair{"a"sv, std::vector<int>{}},
             std::pair{"b"sv, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"c"sv, std::vector<int>({1, 2, 3, 4, 5})}});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(m, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                m, 700, nullptr)));
    }
    {
        boost::container::flat_map<std::string_view, std::vector<int>> m(
            {std::pair{"a"sv, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"b"sv, std::vector<int>{}},
             std::pair{"c"sv, std::vector<int>({1, 2, 3, 4, 5})}});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(m, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                m, 7, nullptr)));
    }
    {
        boost::container::flat_map<std::string_view, std::vector<int>> m(
            {std::pair{"a"sv, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"b"sv, std::vector<int>{}},
             std::pair{"c"sv, std::vector<int>({1, 2, 3, 4, 5})}});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(m, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                m, 700, nullptr)));
    }
    {
        boost::container::flat_map<std::string_view, std::vector<int>> m(
            {std::pair{"a"sv, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"b"sv, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"c"sv, std::vector<int>{}}});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(m, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                m, 7, nullptr)));
    }
    {
        boost::container::flat_map<std::string_view, std::vector<int>> m(
            {std::pair{"a"sv, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"b"sv, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"c"sv, std::vector<int>{}}});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(m, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                m, 700, nullptr)));
    }
    {
        boost::container::flat_map<std::string_view, std::vector<int>> m(
            {std::pair{"a"sv, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"b"sv, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"c"sv, std::vector<int>({1, 2, 3, 4, 5})}});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(m, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                m, 7, nullptr)));
    }
    {
        boost::container::flat_map<std::string_view, std::vector<int>> m(
            {std::pair{"a"sv, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"b"sv, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"c"sv, std::vector<int>({1, 2, 3, 4, 5})}});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(m, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                m, 700, nullptr)));
    }

    // without field
    // int
    {
        boost::container::flat_map<std::string_view, int> m;
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(m, 1, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                m, 1, nullptr)));
    }
    {
        boost::container::flat_map<std::string_view, int> m;
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(m, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                m, 7, nullptr)));
    }
    {
        boost::container::flat_map<std::string_view, int> m;
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(m, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                m, 700, nullptr)));
    }
    {
        boost::container::flat_map<std::string_view, int> m({
            std::pair{"one"sv, 1},
            std::pair{"two"sv, 2},
            std::pair{"three"sv, 3},
            std::pair{"four"sv, 4},
            std::pair{"five"sv, 5},
        });
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(m, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                m, 7, nullptr)));
    }
    {
        boost::container::flat_map<std::string_view, int> m({
            std::pair{"one"sv, 1},
            std::pair{"two"sv, 2},
            std::pair{"three"sv, 3},
            std::pair{"four"sv, 4},
            std::pair{"five"sv, 5},
        });
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(m, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                m, 700, nullptr)));
    }
    // double
    {
        boost::container::flat_map<std::string_view, double> m;
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(m, 1, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                m, 1, nullptr)));
    }
    {
        boost::container::flat_map<std::string_view, double> m;
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(m, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                m, 7, nullptr)));
    }
    {
        boost::container::flat_map<std::string_view, double> m;
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(m, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                m, 700, nullptr)));
    }
    {
        boost::container::flat_map<std::string_view, int> m({
            std::pair{"one"sv, 1.0},
            std::pair{"two"sv, 2.0},
            std::pair{"three"sv, 3.0},
            std::pair{"four"sv, 4.0},
            std::pair{"five"sv, 5.0},
        });
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(m, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                m, 7, nullptr)));
    }
    {
        boost::container::flat_map<std::string_view, int> m({
            std::pair{"one"sv, 1.0},
            std::pair{"two"sv, 2.0},
            std::pair{"three"sv, 3.0},
            std::pair{"four"sv, 4.0},
            std::pair{"five"sv, 5.0},
        });
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(m, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                m, 700, nullptr)));
    }
    // vector<int>
    {
        boost::container::flat_map<std::string_view, std::vector<int>> m;
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(m, 1, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                m, 1, nullptr)));
    }
    {
        boost::container::flat_map<std::string_view, std::vector<int>> m;
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(m, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                m, 7, nullptr)));
    }
    {
        boost::container::flat_map<std::string_view, std::vector<int>> m;
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(m, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                m, 700, nullptr)));
    }
    {
        boost::container::flat_map<std::string_view, std::vector<int>> m(
            {std::pair{"a"sv, std::vector<int>{}},
             std::pair{"b"sv, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"c"sv, std::vector<int>({1, 2, 3, 4, 5})}});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(m, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                m, 7, nullptr)));
    }
    {
        boost::container::flat_map<std::string_view, std::vector<int>> m(
            {std::pair{"a"sv, std::vector<int>{}},
             std::pair{"b"sv, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"c"sv, std::vector<int>({1, 2, 3, 4, 5})}});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(m, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                m, 700, nullptr)));
    }
    {
        boost::container::flat_map<std::string_view, std::vector<int>> m(
            {std::pair{"a"sv, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"b"sv, std::vector<int>{}},
             std::pair{"c"sv, std::vector<int>({1, 2, 3, 4, 5})}});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(m, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                m, 7, nullptr)));
    }
    {
        boost::container::flat_map<std::string_view, std::vector<int>> m(
            {std::pair{"a"sv, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"b"sv, std::vector<int>{}},
             std::pair{"c"sv, std::vector<int>({1, 2, 3, 4, 5})}});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(m, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                m, 700, nullptr)));
    }
    {
        boost::container::flat_map<std::string_view, std::vector<int>> m(
            {std::pair{"a"sv, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"b"sv, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"c"sv, std::vector<int>{}}});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(m, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                m, 7, nullptr)));
    }
    {
        boost::container::flat_map<std::string_view, std::vector<int>> m(
            {std::pair{"a"sv, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"b"sv, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"c"sv, std::vector<int>{}}});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(m, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                m, 700, nullptr)));
    }
    {
        boost::container::flat_map<std::string_view, std::vector<int>> m(
            {std::pair{"a"sv, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"b"sv, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"c"sv, std::vector<int>({1, 2, 3, 4, 5})}});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(m, 7, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                m, 7, nullptr)));
    }
    {
        boost::container::flat_map<std::string_view, std::vector<int>> m(
            {std::pair{"a"sv, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"b"sv, std::vector<int>({1, 2, 3, 4, 5})},
             std::pair{"c"sv, std::vector<int>({1, 2, 3, 4, 5})}});
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(m, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                m, 700, nullptr)));
    }
}

TEST(serialization_tests, serialize_impl_nonnested_message_counts)
{
    using namespace detail;
    using namespace std::literals;

    // with field
    {
        detail::by_hand::unit_t x = {
            .design_id = 142, .design_owner = 800, .health = 0};

        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                x, 700, nullptr)));
    }

    {
        detail::by_hand::unit_t x = {
            .design_id = 142, .design_owner = 0, .health = 190};

        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                x, 700, nullptr)));
    }

    {
        detail::by_hand::unit_t x = {
            .design_id = 0, .design_owner = 800, .health = 190};

        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                x, 700, nullptr)));
    }

    // without field
    {
        detail::by_hand::unit_t x = {
            .design_id = 142, .design_owner = 800, .health = 0};

        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                x, 700, nullptr)));
    }

    {
        detail::by_hand::unit_t x = {
            .design_id = 142, .design_owner = 0, .health = 190};

        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                x, 700, nullptr)));
    }

    {
        detail::by_hand::unit_t x = {
            .design_id = 0, .design_owner = 800, .health = 190};

        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                x, 700, nullptr)));
    }
}

TEST(serialization_tests, serialize_impl_nested_message_counts)
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
        detail::by_hand::fleet_t x = {
            .id = 189,
            .mission = detail::by_hand::mission_t::expand_colony,
            .units = {u1, u2, u3},
            .fuel = 27e6,
            .rounds = 1337,
            .missiles = 42,
            .fighters = 13};
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                x, 700, nullptr)));
    }

    {
        detail::by_hand::fleet_t x = {
            .id = 189,
            .mission = detail::by_hand::mission_t::expand_colony,
            .units = {u1, u3, u2},
            .fuel = 27e6,
            .rounds = 1337,
            .missiles = 42,
            .fighters = 13};
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                x, 700, nullptr)));
    }

    {
        detail::by_hand::fleet_t x = {
            .id = 189,
            .mission = detail::by_hand::mission_t::expand_colony,
            .units = {u2, u3, u1},
            .fuel = 27e6,
            .rounds = 1337,
            .missiles = 42,
            .fighters = 13};
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::write>(x, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::write>(
                x, 700, nullptr)));
    }

    // without field
    {
        detail::by_hand::fleet_t x = {
            .id = 189,
            .mission = detail::by_hand::mission_t::expand_colony,
            .units = {u1, u2, u3},
            .fuel = 27e6,
            .rounds = 1337,
            .missiles = 42,
            .fighters = 13};
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                x, 700, nullptr)));
    }

    {
        detail::by_hand::fleet_t x = {
            .id = 189,
            .mission = detail::by_hand::mission_t::expand_colony,
            .units = {u1, u3, u2},
            .fuel = 27e6,
            .rounds = 1337,
            .missiles = 42,
            .fighters = 13};
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                x, 700, nullptr)));
    }

    {
        detail::by_hand::fleet_t x = {
            .id = 189,
            .mission = detail::by_hand::mission_t::expand_colony,
            .units = {u2, u3, u1},
            .fuel = 27e6,
            .rounds = 1337,
            .missiles = 42,
            .fighters = 13};
        std::ostringstream oss(std::ios::binary);
        auto const initial_tellp = oss.tellp();
        serialize_impl<ser_op::write, ser_field_op::dont_write>(x, 700, &oss);
        auto const bytes_written = oss.tellp() - initial_tellp;
        EXPECT_EQ(
            bytes_written,
            (serialize_impl<ser_op::count, ser_field_op::dont_write>(
                x, 700, nullptr)));
    }
}
