#include <model.hpp>
#include <json2pb.h>
#include <generate_galaxy.hpp>

#include <gtest/gtest.h>


void print_time(auto prev_time)
{
    auto const now = std::chrono::high_resolution_clock::now();
    auto const elapsed = now - prev_time;
    auto const ms =
        std::chrono::duration_cast < std::chrono::milliseconds>(elapsed);
    std::cout << std::format(" ({})\n", ms);
}

TEST(model_tests, generate_save_load)
{
    std::cout << "generate_save_load test\n";

    model m;
    concurrent_queue<int> q;
    std::atomic_bool complete = false;
    std::cout << "generating";
    auto start_time = std::chrono::high_resolution_clock::now();
    m.generate_galaxy(default_game_start_params(), q, complete);
    print_time(start_time);

    std::filesystem::path path = "save_0";
    std::filesystem::remove(path);

    std::cout << "scheduling save";
    start_time = std::chrono::high_resolution_clock::now();
    m.save(path);
    print_time(start_time);

    auto const pre_save_game_state = m.game_state();

    // Wait for it to be written before reading.
    std::cout << "waiting for save file to appear";
    start_time = std::chrono::high_resolution_clock::now();
    while (!std::filesystem::exists(path)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    print_time(start_time);

    std::cout << "loading";
    start_time = std::chrono::high_resolution_clock::now();
    m.load(path);
    print_time(start_time);

    auto const post_load_game_state = m.game_state();

    std::cout << "comparing game_states";
    start_time = std::chrono::high_resolution_clock::now();
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
    print_time(start_time);
}
