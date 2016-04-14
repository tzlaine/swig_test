#include <model.hpp>
#include <model_data.hpp>
#include <json2pb.h>
#include <gtest/gtest.h>

// This horrible business works around the fact that the MS compiler
// chokes on string literals that are "too long".
#ifdef _MSC_VER
#include <fstream>

const char* nations_json_string = nullptr;
const char* map_json_string = nullptr;
const char* oob_json_string = nullptr;
const char* units_json_string = nullptr;
const char* scenario_json_string = nullptr;

std::string file_slurp (const char* filename)
{
    std::string retval;
    std::ifstream ifs(filename);
    std::getline(ifs, retval, '\0');
    return retval;
}
#else
#include <test_data.hpp>
#endif

TEST(init_model_ops, test_init_nations)
{
    EXPECT_TRUE(init_nations(nations_json_string));
    EXPECT_TRUE(init_nations(nations_json_string));

    EXPECT_TRUE(reset_model());
}

TEST(init_model_ops, test_init_units)
{
    EXPECT_TRUE(init_unit_defs(units_json_string));
    EXPECT_TRUE(init_unit_defs(units_json_string));

    EXPECT_TRUE(reset_model());
}

TEST(init_model_ops, test_init_scenario)
{
    EXPECT_FALSE(init_scenario(scenario_json_string));

    EXPECT_TRUE(init_nations(nations_json_string));
    EXPECT_TRUE(init_scenario(scenario_json_string));

    EXPECT_TRUE(init_scenario(scenario_json_string));

    EXPECT_TRUE(reset_model());
}

TEST(init_model_ops, test_init_model)
{
    EXPECT_FALSE(init_model(map_json_string, oob_json_string));

    EXPECT_TRUE(init_unit_defs(units_json_string));

    EXPECT_FALSE(init_model(map_json_string, oob_json_string));

    EXPECT_TRUE(init_nations(nations_json_string));

    EXPECT_FALSE(init_model(map_json_string, oob_json_string));

    EXPECT_TRUE(init_scenario(scenario_json_string));
    EXPECT_TRUE(init_model(map_json_string, oob_json_string));

    EXPECT_TRUE(reset_model());
}

TEST(init_model_ops, test_null_string_init)
{
    EXPECT_FALSE(init_nations(nullptr));
    EXPECT_FALSE(init_unit_defs(nullptr));

    EXPECT_TRUE(init_nations(nations_json_string));

    EXPECT_FALSE(init_scenario(nullptr));
    EXPECT_TRUE(init_scenario(scenario_json_string));

    EXPECT_TRUE(init_unit_defs(units_json_string));

    EXPECT_FALSE(init_model(nullptr, oob_json_string));
    EXPECT_FALSE(init_model(map_json_string, nullptr));

    EXPECT_TRUE(init_model(map_json_string, oob_json_string));

    EXPECT_TRUE(reset_model());
}

TEST(init_model_ops, test_empty_string_init)
{
    EXPECT_FALSE(init_nations(""));
    EXPECT_FALSE(init_unit_defs(""));

    EXPECT_TRUE(init_nations(nations_json_string));

    EXPECT_FALSE(init_scenario(""));
    EXPECT_TRUE(init_scenario(scenario_json_string));

    EXPECT_TRUE(init_unit_defs(units_json_string));

    EXPECT_FALSE(init_model("", oob_json_string));
    EXPECT_FALSE(init_model(map_json_string, ""));

    EXPECT_TRUE(init_model(map_json_string, oob_json_string));

    EXPECT_TRUE(reset_model());
}

TEST(init_model_ops, test_get_nations)
{
    EXPECT_TRUE(init_nations(nations_json_string));

    unsigned char* bytes;
    int size;
    EXPECT_TRUE(get_nations((void**)&bytes, &size));

    unsigned char* const inital_bytes = bytes;
    nations_t from_get_nations = nations_t_from_bin(bytes);

    EXPECT_EQ(bytes, inital_bytes + size);

    message::nations_t nations_msg;
    json2pb(nations_msg, nations_json_string, strlen(nations_json_string), map_encoding_t::compact);
    nations_t from_string = from_protobuf(nations_msg);

    ASSERT_EQ(from_get_nations.nations.size(), from_string.nations.size());
    auto from_get_nations_it = from_get_nations.nations.begin();
    auto from_string_it = from_string.nations.begin();
    EXPECT_EQ(from_get_nations_it->first, from_string_it->first);
    const nation_t& first_nation_from_get_nations = from_get_nations_it->second;
    const nation_t& first_nation_from_string = from_string_it->second;
    EXPECT_EQ(first_nation_from_get_nations.short_name, first_nation_from_string.short_name);

    // capital
    const capital_t& capital_from_get_nations = first_nation_from_get_nations.capital;
    const capital_t& capital_from_string = first_nation_from_string.capital;
    ASSERT_EQ(capital_from_get_nations.hexes.size(), capital_from_string.hexes.size());
    for (std::size_t i = 0; i < capital_from_get_nations.hexes.size(); ++i) {
        EXPECT_EQ(capital_from_get_nations.hexes[i].coord, capital_from_string.hexes[i].coord);
        ASSERT_EQ(capital_from_get_nations.hexes[i].zones.size(), capital_from_string.hexes[i].zones.size());
        for (std::size_t j = 0; j < capital_from_get_nations.hexes[i].zones.size(); ++j) {
            EXPECT_EQ(capital_from_get_nations.hexes[i].zones[j].name, capital_from_string.hexes[i].zones[j].name);
            ASSERT_EQ(capital_from_get_nations.hexes[i].zones[j].features.size(), capital_from_string.hexes[i].zones[j].features.size());
            for (std::size_t k = 0; k < capital_from_get_nations.hexes[i].zones[j].features.size(); ++k) {
                EXPECT_EQ(capital_from_get_nations.hexes[i].zones[j].features[k], capital_from_string.hexes[i].zones[j].features[k]);
            }
        }
    }

    EXPECT_EQ(first_nation_from_get_nations.free_strategic_moves, first_nation_from_string.free_strategic_moves);
    EXPECT_EQ(first_nation_from_get_nations.capital_star_points, first_nation_from_string.capital_star_points);

    // offmap_possesions
    EXPECT_EQ(first_nation_from_get_nations.offmap_possesions.provinces, first_nation_from_string.offmap_possesions.provinces);
    EXPECT_EQ(first_nation_from_get_nations.offmap_possesions.mins, first_nation_from_string.offmap_possesions.mins);
    EXPECT_EQ(first_nation_from_get_nations.offmap_possesions.majs, first_nation_from_string.offmap_possesions.majs);
    EXPECT_EQ(first_nation_from_get_nations.offmap_possesions.survey_ships, first_nation_from_string.offmap_possesions.survey_ships);
    EXPECT_EQ(first_nation_from_get_nations.offmap_possesions.cannot_build_offmap_capital, first_nation_from_string.offmap_possesions.cannot_build_offmap_capital);
    EXPECT_EQ(first_nation_from_get_nations.offmap_possesions.old_shipyard, first_nation_from_string.offmap_possesions.old_shipyard);

    // Should not match (filled in by post-parse function).
    // EXPECT_EQ(first_nation_from_get_nations.nation_id, first_nation_from_string.nation_id);

    EXPECT_TRUE(reset_model());
}

TEST(init_model_ops, test_nations_bin_encoding)
{
    message::nations_t nations_msg;
    json2pb(nations_msg, nations_json_string, strlen(nations_json_string), map_encoding_t::compact);
    nations_t from_string = from_protobuf(nations_msg);

    std::vector<unsigned char> encoded_bytes;
    to_bin(from_string, encoded_bytes);

    unsigned char* bytes = &encoded_bytes[0];
    unsigned char* const inital_bytes = bytes;
    nations_t from_encoding = nations_t_from_bin(bytes);

    EXPECT_EQ(bytes, inital_bytes + encoded_bytes.size());

    ASSERT_EQ(from_encoding.nations.size(), from_string.nations.size());
    auto from_encoding_it = from_encoding.nations.begin();
    for (const auto& string_nation_pair : from_string.nations) {
        const auto& encoding_nation_pair = *from_encoding_it++;

        EXPECT_EQ(encoding_nation_pair.first, string_nation_pair.first);
        const nation_t& nation_from_encoding = encoding_nation_pair.second;
        const nation_t& nation_from_string = string_nation_pair.second;
        EXPECT_EQ(nation_from_encoding.short_name, nation_from_string.short_name);

        // capital
        const capital_t& capital_from_encoding = nation_from_encoding.capital;
        const capital_t& capital_from_string = nation_from_string.capital;
        ASSERT_EQ(capital_from_encoding.hexes.size(), capital_from_string.hexes.size());
        for (std::size_t i = 0; i < capital_from_encoding.hexes.size(); ++i) {
            EXPECT_EQ(capital_from_encoding.hexes[i].coord, capital_from_string.hexes[i].coord);
            ASSERT_EQ(capital_from_encoding.hexes[i].zones.size(), capital_from_string.hexes[i].zones.size());
            for (std::size_t j = 0; j < capital_from_encoding.hexes[i].zones.size(); ++j) {
                EXPECT_EQ(capital_from_encoding.hexes[i].zones[j].name, capital_from_string.hexes[i].zones[j].name);
                ASSERT_EQ(capital_from_encoding.hexes[i].zones[j].features.size(), capital_from_string.hexes[i].zones[j].features.size());
                for (std::size_t k = 0; k < capital_from_encoding.hexes[i].zones[j].features.size(); ++k) {
                    EXPECT_EQ(capital_from_encoding.hexes[i].zones[j].features[k], capital_from_string.hexes[i].zones[j].features[k]);
                }
            }
        }

        EXPECT_EQ(nation_from_encoding.free_strategic_moves, nation_from_string.free_strategic_moves);
        EXPECT_EQ(nation_from_encoding.capital_star_points, nation_from_string.capital_star_points);

        // offmap_possesions
        EXPECT_EQ(nation_from_encoding.offmap_possesions.provinces, nation_from_string.offmap_possesions.provinces);
        EXPECT_EQ(nation_from_encoding.offmap_possesions.mins, nation_from_string.offmap_possesions.mins);
        EXPECT_EQ(nation_from_encoding.offmap_possesions.majs, nation_from_string.offmap_possesions.majs);
        EXPECT_EQ(nation_from_encoding.offmap_possesions.survey_ships, nation_from_string.offmap_possesions.survey_ships);
        EXPECT_EQ(nation_from_encoding.offmap_possesions.cannot_build_offmap_capital, nation_from_string.offmap_possesions.cannot_build_offmap_capital);
        EXPECT_EQ(nation_from_encoding.offmap_possesions.old_shipyard, nation_from_string.offmap_possesions.old_shipyard);

        EXPECT_EQ(nation_from_encoding.nation_id, nation_from_string.nation_id);
    }
}

int main(int argc, char **argv)
{
#ifdef _MSC_VER
    std::string nations_json_string_;
    std::string map_json_string_;
    std::string oob_json_string_;
    std::string units_json_string_;
    std::string scenario_json_string_;

    if (1 < argc) {
        nations_json_string_ = file_slurp(argv[1]);
	nations_json_string = nations_json_string_.c_str();
    }
    if (2 < argc) {
        map_json_string_ = file_slurp(argv[2]);
	map_json_string = map_json_string_.c_str();
    }
    if (3 < argc) {
        oob_json_string_ = file_slurp(argv[3]);
	oob_json_string = oob_json_string_.c_str();
    }
    if (4 < argc) {
        units_json_string_ = file_slurp(argv[4]);
	units_json_string = units_json_string_.c_str();
    }
    if (5 < argc) {
        scenario_json_string_ = file_slurp(argv[5]);
	scenario_json_string = scenario_json_string_.c_str();
    }
#endif

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
