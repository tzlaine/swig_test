#include <proximity_grid.hpp>
#include <model.hpp>
#include <generate_galaxy.hpp>

#include <gtest/gtest.h>


void generate_hexes_and_nations(
    game_state_t & gs, int nations, std::pair<int, int> allied)
{
    generation::detail::g_skip_system_generation_for_testing = true;
    game_start_params_t params = default_game_start_params();
    generation::generate_galaxy(params, gs);
    gs.nations.resize(nations);
    int i = 0;
    for (auto & nation : gs.nations) {
        nation.id = i;
        ++i;
    }
    gs.alliances.resize(1u);
    form_alliance(gs, allied.first, allied.second);
}

TEST(proximity_grid_tests, default_ctor)
{
    {
        proximity_grid<fleet_t const> grid;
    }
    {
        proximity_grid<fleet_t> grid;
    }
}

TEST(proximity_grid_tests, ctor)
{
    game_state_t gs = {0};
    {
        proximity_grid<fleet_t const> grid(gs);
    }
    {
        proximity_grid<fleet_t> grid(gs);
    }
}

TEST(proximity_grid_tests, insert)
{
    game_state_t gs = {0};
    fleet_t f = {0};
    {
        proximity_grid<fleet_t const> grid(gs);
        grid.insert(f);
        grid.insert(&f);
    }
    {
        proximity_grid<fleet_t> grid(gs);
        grid.insert(f);
        grid.insert(&f);
    }
}

TEST(proximity_grid_tests, erase)
{
    game_state_t gs = {0};
    fleet_t f = {0};
    {
        proximity_grid<fleet_t const> grid(gs);
        grid.erase(f);
        grid.erase(&f);
    }
    {
        proximity_grid<fleet_t> grid(gs);
        grid.erase(f);
        grid.erase(&f);
    }
}

std::ostream & operator<<(std::ostream & os, int_point pt)
{
    os << '{' << pt.x << ", " << pt.y << '}';
    return os;
}

TEST(proximity_grid_tests, detail)
{
    {
        double const max_radius = 0.1;
        double const cell_size = 1.0;
        std::vector<int_point> const offsets =
            detail::max_radius_offsets(max_radius, cell_size);
        EXPECT_EQ(offsets.size(), 9u);
        EXPECT_EQ(
            offsets,
            std::vector<int_point>(
                {{-1, -1},
                 {0, -1},
                 {1, -1},
                 {-1, 0},
                 {0, 0},
                 {1, 0},
                 {-1, 1},
                 {0, 1},
                 {1, 1}}));
    }
    {
        double const max_radius = 1.0;
        double const cell_size = 1.0;
        std::vector<int_point> const offsets =
            detail::max_radius_offsets(max_radius, cell_size);
        EXPECT_EQ(offsets.size(), 9u);
        EXPECT_EQ(
            offsets,
            std::vector<int_point>(
                {{-1, -1},
                 {0, -1},
                 {1, -1},
                 {-1, 0},
                 {0, 0},
                 {1, 0},
                 {-1, 1},
                 {0, 1},
                 {1, 1}}));
    }
    {
        double const max_radius = 1.1;
        double const cell_size = 1.0;
        std::vector<int_point> const offsets =
            detail::max_radius_offsets(max_radius, cell_size);
        EXPECT_EQ(offsets.size(), 21u);
        EXPECT_EQ(
            offsets,
            std::vector<int_point>(
                {{-1, -2}, {0, -2}, {1, -2}, {-2, -1}, {-1, -1}, {0, -1},
                 {1, -1},  {2, -1}, {-2, 0}, {-1, 0},  {0, 0},   {1, 0},
                 {2, 0},   {-2, 1}, {-1, 1}, {0, 1},   {1, 1},   {2, 1},
                 {-1, 2},  {0, 2},  {1, 2}}));
    }
}

TEST(proximity_grid_tests, detect_proximity)
{
    detail::g_testing_detection_dist = 1.0;

    int const own_id = 2;
    int const nations = 4;
    std::pair const alliance(own_id, 1);

    game_state_t gs = {0};
    generate_hexes_and_nations(gs, nations, alliance);

    std::vector<fleet_t> own_fleets(
        {{.id = {own_id, 0},
          .position{.world_pos_x = 0.0, .world_pos_y = 0.0}}});
    std::vector<fleet_t> foreign_fleets(
        {{.id = {0, 0}, .position{.world_pos_x = 0.0, .world_pos_y = 1.0}},
         {.id = {1, 0}, .position{.world_pos_x = 0.0, .world_pos_y = 2.0}},
         {.id = {3, 0}, .position{.world_pos_x = 0.0, .world_pos_y = 3.0}},
         {.id = {3, 0}, .position{.world_pos_x = 0.0, .world_pos_y = 4.0}}});

    {
        proximity_grid<fleet_t const, 2> grid(gs);
        grid.insert(own_fleets.front());
        for (auto & f : foreign_fleets) {
            grid.insert(f);
        }

        {
            std::vector<fleet_t> empty_fleets;
            std::vector<fleet_t const *> result;
            grid.gather_visible_objects(
                result, own_id, empty_fleets, gs, find_visible::non_allied);
            EXPECT_TRUE(result.empty());
            grid.gather_visible_objects(
                result, own_id, empty_fleets, gs, find_visible::all);
            EXPECT_TRUE(result.empty());
        }
        {
            std::vector<fleet_t const *> empty_fleets;
            std::vector<fleet_t const *> result;
            grid.gather_visible_objects(
                result, own_id, empty_fleets, gs, find_visible::non_allied);
            EXPECT_TRUE(result.empty());
            grid.gather_visible_objects(
                result, own_id, empty_fleets, gs, find_visible::all);
            EXPECT_TRUE(result.empty());
        }
        {
            detail::g_testing_detection_dist = 0.99;
            std::vector<fleet_t const *> result;
            grid.gather_visible_objects(
                result, own_id, own_fleets, gs, find_visible::non_allied);
            EXPECT_EQ(result.size(), 0u);
            result.clear();
            grid.gather_visible_objects(
                result, own_id, own_fleets, gs, find_visible::all);
            EXPECT_EQ(result.size(), 1u);
            EXPECT_EQ(
                result, std::vector<fleet_t const *>({&own_fleets[0]}));
        }
        {
            detail::g_testing_detection_dist = 1.01;
            std::vector<fleet_t const *> result;
            grid.gather_visible_objects(
                result, own_id, own_fleets, gs, find_visible::non_allied);
            EXPECT_EQ(result.size(), 1u);
            EXPECT_EQ(
                result, std::vector<fleet_t const *>({&foreign_fleets[0]}));
            result.clear();
            grid.gather_visible_objects(
                result, own_id, own_fleets, gs, find_visible::all);
            EXPECT_EQ(result.size(), 2u);
            EXPECT_EQ(
                result,
                std::vector<fleet_t const *>(
                    {&own_fleets[0], &foreign_fleets[0]}));
        }
        {
            detail::g_testing_detection_dist = 2.01;
            std::vector<fleet_t const *> result;
            grid.gather_visible_objects(
                result, own_id, own_fleets, gs, find_visible::non_allied);
            EXPECT_EQ(result.size(), 1u);
            EXPECT_EQ(
                result, std::vector<fleet_t const *>({&foreign_fleets[0]}));
            result.clear();
            grid.gather_visible_objects(
                result, own_id, own_fleets, gs, find_visible::all);
            EXPECT_EQ(result.size(), 3u);
            EXPECT_EQ(
                result,
                std::vector<fleet_t const *>(
                    {&own_fleets[0], &foreign_fleets[0], &foreign_fleets[1]}));
        }
        {
            detail::g_testing_detection_dist = 3.01;
            std::vector<fleet_t const *> result;
            grid.gather_visible_objects(
                result, own_id, own_fleets, gs, find_visible::non_allied);
            EXPECT_EQ(result.size(), 2u);
            EXPECT_EQ(
                result,
                std::vector<fleet_t const *>(
                    {&foreign_fleets[0], &foreign_fleets[2]}));
            result.clear();
            grid.gather_visible_objects(
                result, own_id, own_fleets, gs, find_visible::all);
            EXPECT_EQ(result.size(), 4u);
            EXPECT_EQ(
                result,
                std::vector<fleet_t const *>(
                    {&own_fleets[0],
                     &foreign_fleets[0],
                     &foreign_fleets[1],
                     &foreign_fleets[2]}));
        }
        {
            detail::g_testing_detection_dist = 4.01;
            std::vector<fleet_t const *> result;
            grid.gather_visible_objects(
                result, own_id, own_fleets, gs, find_visible::non_allied);
            EXPECT_EQ(result.size(), 3u);
            EXPECT_EQ(
                result,
                std::vector<fleet_t const *>(
                    {&foreign_fleets[0],
                     &foreign_fleets[2],
                     &foreign_fleets[3]}));
            result.clear();
            grid.gather_visible_objects(
                result, own_id, own_fleets, gs, find_visible::all);
            EXPECT_EQ(result.size(), 5u);
            EXPECT_EQ(
                result,
                std::vector<fleet_t const *>(
                    {&own_fleets[0],
                     &foreign_fleets[0],
                     &foreign_fleets[1],
                     &foreign_fleets[2],
                     &foreign_fleets[3]}));
        }
    }
    {
        proximity_grid<fleet_t, 2> grid(gs);
        grid.insert(own_fleets.front());
        for (auto & f : foreign_fleets) {
            grid.insert(f);
        }

        {
            std::vector<fleet_t> empty_fleets;
            std::vector<fleet_t *> result;
            grid.gather_visible_objects(
                result, own_id, empty_fleets, gs, find_visible::non_allied);
            EXPECT_TRUE(result.empty());
            grid.gather_visible_objects(
                result, own_id, empty_fleets, gs, find_visible::all);
            EXPECT_TRUE(result.empty());
        }
        {
            std::vector<fleet_t *> empty_fleets;
            std::vector<fleet_t *> result;
            grid.gather_visible_objects(
                result, own_id, empty_fleets, gs, find_visible::non_allied);
            EXPECT_TRUE(result.empty());
            grid.gather_visible_objects(
                result, own_id, empty_fleets, gs, find_visible::all);
            EXPECT_TRUE(result.empty());
        }
    }
}
