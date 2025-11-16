#include <model.hpp>
#include <json2pb.h>
#include <generate_galaxy.hpp>

#include <gtest/gtest.h>


TEST(model_tests, generate_save_load)
{
    std::cout << "generate_save_load test\n";

    model m;
    concurrent_queue<int> q;
    std::atomic_bool complete = false;
    std::cout << "generating\n";
    m.generate_galaxy(default_game_start_params(), q, complete);

    std::filesystem::path path = "save_0";
    std::filesystem::remove(path);

    std::cout << "saving\n";
    m.save(path);

    auto const pre_save_game_state = m.game_state();

    // Wait for it to be written before reading.
    std::cout << "waiting for save file to appear\n";
    while (!std::filesystem::exists(path)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "loading\n";
    m.load(path);

    auto const post_load_game_state = m.game_state();

    std::cout << "comparing game_states\n";
    game_state_t const & before = *pre_save_game_state;
    game_state_t const & after = *post_load_game_state;
    EXPECT_EQ(before.map_width, after.map_width);
    EXPECT_EQ(before.map_height, after.map_height);
    EXPECT_EQ(before.hexes.size(), after.hexes.size());
    EXPECT_EQ(before.hexes, after.hexes);
    EXPECT_EQ(before.systems.size(), after.systems.size());
    EXPECT_EQ(before.systems, after.systems);
    EXPECT_EQ(before.planets.size(), after.planets.size());
    EXPECT_EQ(before.planets, after.planets);
    EXPECT_EQ(before.nations.size(), after.nations.size());
    EXPECT_EQ(before.nations, after.nations);
}
