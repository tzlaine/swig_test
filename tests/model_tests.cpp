#include <model.hpp>
#include <gtest/gtest.h>

// This horrible business works around the fact that the MS compiler
// chokes on string literals that are "too long".
#ifdef _MSC_VER
#include <fstream>

const char* nations_json_string = nullptr;
const char* map_json_string = nullptr;
const char* oob_json_string = nullptr;
const char* units_json_string = nullptr;

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

TEST(whole_model_ops, test_init_nations)
{
    EXPECT_TRUE(init_nations(nations_json_string));
}

TEST(whole_model_ops, test_init_model)
{
    EXPECT_TRUE(init_nations(nations_json_string));
    EXPECT_TRUE(init_model(map_json_string, oob_json_string));
}

TEST(whole_model_ops, test_init_units)
{
    EXPECT_TRUE(init_unit_defs(units_json_string));
}

TEST(whole_model_ops, test_init_scenario)
{
    EXPECT_TRUE(init_nations(nations_json_string));
    EXPECT_TRUE(init_scenario(scenario_json_string));
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
