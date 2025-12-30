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
    game_start_params_t const params = game_start_params_t{
        .habitable_systems_per_hex_mean = 5.0,
        .habitable_systems_per_hex_plus_minus = 2.0,
        .systems_per_hex = 20,
        .map_height = 11};
    m.generate_galaxy(params, q, complete);
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

TEST(model_tests, rng_reseeding)
{
    std::vector<unsigned int> seed = rng_state_seed();
    EXPECT_EQ(seed.size(), 16u);
    std::ranges::fill(seed, 0u);

    // Odd pattern.
#if 0
    seed[0] = 1; // affects both
    seed[1] = 2; // affects both
    seed[2] = 3; // affects 64 only
    seed[3] = 4; // affects 64 only
    seed[4] = 5; // affects no effect
    seed[5] = 6; // affects no effect
    seed[6] = 7; // affects 32 only
    seed[7] = 8; // affects 32 only
#if 0 // has no effect
    seed[8] = 9;
    seed[9] = 10;
    seed[10] = 11;
    seed[11] = 12;
#endif
#if 0 // affects 64 only
    seed[12] = 13;
    seed[13] = 14;
    seed[14] = 15;
    seed[15] = 16;
#endif
#endif

    reset_rng_state(seed);

    static_assert(4u < sizeof(long long));

    std::uniform_int_distribution<int> dist_i(0, 1000);
    std::vector<int> four_from_pcg32(4);
    random_numbers(dist_i, four_from_pcg32);

    std::uniform_int_distribution<long long> dist_ll(0, 1000);
    std::vector<long long> four_from_pcg64(4);
    random_numbers(dist_ll, four_from_pcg64);

    std::vector<int> const expected_i({894, 217, 360, 375});
    std::vector<long long> const expected_ll({832, 908, 228, 481});
    EXPECT_EQ(four_from_pcg32, expected_i);
    EXPECT_EQ(four_from_pcg64, expected_ll);

    {
        auto state = detail::rng_state_from(0u);
        std::vector<int> four_from_pcg32(4);
        random_numbers(dist_i, four_from_pcg32, state);
        std::vector<long long> four_from_pcg64(4);
        random_numbers(dist_ll, four_from_pcg64, state);
        EXPECT_EQ(four_from_pcg32, expected_i);
        EXPECT_EQ(four_from_pcg64, expected_ll);
    }

    {
        auto state = detail::rng_state_from(0u, 0u);
        std::vector<int> four_from_pcg32(4);
        random_numbers(dist_i, four_from_pcg32, state);
        std::vector<long long> four_from_pcg64(4);
        random_numbers(dist_ll, four_from_pcg64, state);
        EXPECT_EQ(four_from_pcg32, expected_i);
        EXPECT_EQ(four_from_pcg64, expected_ll);
    }

    {
        auto state = detail::rng_state_from(0u, 0u, 0u);
        std::vector<int> four_from_pcg32(4);
        random_numbers(dist_i, four_from_pcg32, state);
        std::vector<long long> four_from_pcg64(4);
        random_numbers(dist_ll, four_from_pcg64, state);
        EXPECT_EQ(four_from_pcg32, expected_i);
        EXPECT_EQ(four_from_pcg64, expected_ll);
    }

    {
        auto state = detail::rng_state_from(0u, 0u, 0u, 0u);
        std::vector<int> four_from_pcg32(4);
        random_numbers(dist_i, four_from_pcg32, state);
        std::vector<long long> four_from_pcg64(4);
        random_numbers(dist_ll, four_from_pcg64, state);
        EXPECT_EQ(four_from_pcg32, expected_i);
        EXPECT_EQ(four_from_pcg64, expected_ll);
    }

    {
        auto state_1 = detail::rng_state_from(1u);
        auto state_2 = detail::rng_state_from(1u, 2u);
        auto state_3 = detail::rng_state_from(1u, 2u, 3u);
        auto state_4 = detail::rng_state_from(1u, 2u, 3u, 4u);

        std::vector<int> prev_four_from_pcg32 = four_from_pcg32;
        std::vector<int> curr_four_from_pcg32(4);
        std::vector<long long> prev_four_from_pcg64 = four_from_pcg64;
        std::vector<long long> curr_four_from_pcg64(4);

        random_numbers(dist_i, curr_four_from_pcg32, state_1);
        random_numbers(dist_ll, curr_four_from_pcg64, state_1);
        EXPECT_NE(prev_four_from_pcg32, curr_four_from_pcg32);
        EXPECT_NE(prev_four_from_pcg64, curr_four_from_pcg64);
        prev_four_from_pcg32 = curr_four_from_pcg32;
        prev_four_from_pcg64 = curr_four_from_pcg64;

        random_numbers(dist_i, curr_four_from_pcg32, state_2);
        random_numbers(dist_ll, curr_four_from_pcg64, state_2);
        EXPECT_NE(prev_four_from_pcg32, curr_four_from_pcg32);
        EXPECT_NE(prev_four_from_pcg64, curr_four_from_pcg64);
        prev_four_from_pcg32 = curr_four_from_pcg32;
        prev_four_from_pcg64 = curr_four_from_pcg64;

        random_numbers(dist_i, curr_four_from_pcg32, state_3);
        random_numbers(dist_ll, curr_four_from_pcg64, state_3);
        EXPECT_NE(prev_four_from_pcg32, curr_four_from_pcg32);
        EXPECT_NE(prev_four_from_pcg64, curr_four_from_pcg64);
        prev_four_from_pcg32 = curr_four_from_pcg32;
        prev_four_from_pcg64 = curr_four_from_pcg64;

        random_numbers(dist_i, curr_four_from_pcg32, state_4);
        random_numbers(dist_ll, curr_four_from_pcg64, state_4);
        EXPECT_NE(prev_four_from_pcg32, curr_four_from_pcg32);
        EXPECT_NE(prev_four_from_pcg64, curr_four_from_pcg64);
    }
}
