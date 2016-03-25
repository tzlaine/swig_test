#include <model.hpp>
#include <data.hpp>
#include <gtest/gtest.h>


TEST(whole_model_ops, init)
{
    EXPECT_TRUE(init_model(nations_pbtxt_string, map_pbtxt_string, oob_pbtxt_string));
}
