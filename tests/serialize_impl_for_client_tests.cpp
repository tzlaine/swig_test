#include <generate_galaxy.hpp>
#include <model.hpp>
#include <serialization.hpp>
#include <task_system.hpp>

#include <gtest/gtest.h>

#include <format>


template<std::ranges::range R>
auto byte_span_of(R const & r)
{
    return std::as_bytes(std::span(r.begin(), r.end()));
}

unit_design_t design(int nation_id, int id)
{
    int const factor = id + 1;
    return {
        .id = nation_and_object_id_t{nation_id, id},
        .hull = factor,
        .armor = factor,
        .propulsion = 3 / factor,
        .weapons = factor,
        .shields = factor,
        .detection = 3 / factor,
        .stealth = 3 / factor,
        .automation = factor,
        .attack = factor,
        .defense = factor,
        .ground_attack = factor};
}

unit_design_t design(int nation_id, int id, game_state_t const & gs)
{
    return gs.nations[nation_id].unit_designs[id];
}

fleet_t fleet(int nation_id)
{
    return fleet_t{
        .id = nation_and_object_id_t{nation_id, 0},
        .mission = mission_t::explore,
        .units =
            {{.id = {nation_id, 0}, .health = 100},
             {.id = {nation_id, 1}, .health = 100},
             {.id = {nation_id, 2}, .health = 100}},
        .fuel = 100.0f,
        .rounds = 100,
        .missiles = 100,
        .fighters = 25,
        .position = {
            .world_pos_x = 1000.0 * nation_id,
            .world_pos_y = 1000.0 * nation_id,
            .system_id = 10 + nation_id,
            .at_permanent_location = true,
            .location_index = 2,
            .object_index = 3,
            .is_garrison = true}};
}

nation_t nation(int nation_id, game_state_t const & gs)
{
    hex_coord_t const home_hc = {.x = 5, .y = 1 + nation_id};
    int const home_hex = to_index(home_hc, gs.map_width);
    int const home_system = gs.hexes[home_hex].first_system;
    int const home_planet = gs.systems[home_system].first_planet;
    nation_t retval = {
        .id = nation_id,
        .unit_designs =
            {design(nation_id, 0), design(nation_id, 0), design(nation_id, 0)},
        .provinces = {},
        .fleets = {fleet(nation_id)},
        .hexes_seen =
            {home_hex,
             to_index(hex_above_right(home_hc), gs.map_width),
             to_index(hex_above(home_hc), gs.map_width),
             to_index(hex_above_left(home_hc), gs.map_width),
             to_index(hex_below_left(home_hc), gs.map_width),
             to_index(hex_below(home_hc), gs.map_width),
             to_index(hex_below_right(home_hc), gs.map_width)},
        .systems_present_in = {home_system},
        .systems_visited = {home_system},
        .planets_present_on = {home_planet},
        .planets_surveyed = {home_planet},
        .foreign_designs_seen =
            {{.nation_id = (nation_id + 1) % 3, .object_id = 1}},
        .foreign_designs_glimpsed = {},
        .defeated = false};
    return retval;
}

auto visible_fleets(int nation_id, game_state_t const & gs)
{
    return std::vector<fleet_t const *>(
        {&gs.nations[(nation_id + 1) % 3].fleets[0],
         &gs.nations[(nation_id + 2) % 3].fleets[0]});
}

game_state_t const gs = [] {
    task_system ts(4);
    game_state_t retval;
    generation::generate_galaxy(default_game_start_params(), retval, &ts);
    retval.nations = {nation(0, retval), nation(1, retval), nation(2, retval)};
    return retval;
}();

TEST(client_serialization_tests, serialize_for_client_single_object)
{
    using namespace detail;

    // unit_design_t
    {
        std::vector<char> serialized;
        ostream_tarray_facade oss(serialized);
        int const nation_id = 0;
        std::vector<fleet_t const *> visible_fleets;
        unit_design_t const design = ::design(nation_id, 0);

        {
            serialized.clear();
            serialize_for_client(
                gs,
                visible_fleets,
                nation_id,
                design,
                visibility_kind::owner,
                &oss);

            unit_design_t client_design = {};
            auto const bytes =
                deserialize_impl(client_design, byte_span_of(serialized));
            EXPECT_TRUE(bytes.empty());
            EXPECT_EQ(client_design, design);
        }
        {
            serialized.clear();
            serialize_for_client(
                gs,
                visible_fleets,
                nation_id,
                design,
                visibility_kind::neutral_or_enemy,
                &oss);

            unit_design_t client_design = {};
            auto const bytes =
                deserialize_impl(client_design, byte_span_of(serialized));
            EXPECT_TRUE(bytes.empty());
            EXPECT_NE(client_design, design);

            EXPECT_EQ(client_design.id, design.id);
            EXPECT_EQ(client_design.hull, design.hull);

            unit_design_t const default_design;
            EXPECT_EQ(client_design.armor, default_design.armor);
            EXPECT_EQ(client_design.propulsion, default_design.propulsion);
            EXPECT_EQ(client_design.weapons, default_design.weapons);
            EXPECT_EQ(client_design.shields, default_design.shields);
            EXPECT_EQ(client_design.detection, default_design.detection);
            EXPECT_EQ(client_design.stealth, default_design.stealth);
            EXPECT_EQ(client_design.automation, default_design.automation);
            EXPECT_EQ(client_design.attack, default_design.attack);
            EXPECT_EQ(client_design.defense, default_design.defense);
            EXPECT_EQ(
                client_design.ground_attack, default_design.ground_attack);
        }
        {
            serialized.clear();
            serialize_for_client(
                gs,
                visible_fleets,
                nation_id,
                design,
                visibility_kind::unseen,
                &oss);
            EXPECT_EQ(byte_span_of(serialized).size(), 1u);
            EXPECT_EQ(byte_span_of(serialized)[0], std::byte(0));
        }
    }

    // unit_t
    {
        std::vector<char> serialized;
        ostream_tarray_facade oss(serialized);
        int const nation_id = 0;
        std::vector<fleet_t const *> visible_fleets;
        unit_t const unit = {.id = {3, 4}, .health = 100};

        {
            serialized.clear();
            serialize_for_client(
                gs,
                visible_fleets,
                nation_id,
                unit,
                visibility_kind::owner,
                &oss);

            unit_t client_unit = {};
            auto const bytes =
                deserialize_impl(client_unit, byte_span_of(serialized));
            EXPECT_TRUE(bytes.empty());
            EXPECT_EQ(client_unit, unit);
        }
        {
            serialized.clear();
            serialize_for_client(
                gs,
                visible_fleets,
                nation_id,
                unit,
                visibility_kind::neutral_or_enemy,
                &oss);

            unit_t client_unit = {};
            auto const bytes =
                deserialize_impl(client_unit, byte_span_of(serialized));
            EXPECT_TRUE(bytes.empty());
            EXPECT_NE(client_unit, unit);

            EXPECT_EQ(client_unit.id, unit.id);

            unit_t const default_unit;
            EXPECT_EQ(client_unit.health, default_unit.health);
        }
        {
            serialized.clear();
            serialize_for_client(
                gs,
                visible_fleets,
                nation_id,
                unit,
                visibility_kind::unseen,
                &oss);
            EXPECT_EQ(byte_span_of(serialized).size(), 1u);
            EXPECT_EQ(byte_span_of(serialized)[0], std::byte(0));
        }
    }

    // fleet_t
    {
        std::vector<char> serialized;
        ostream_tarray_facade oss(serialized);
        int const nation_id = 0;
        std::vector<fleet_t const *> visible_fleets;
        fleet_t const fleet = ::fleet(5);

        {
            serialized.clear();
            serialize_for_client(
                gs,
                visible_fleets,
                nation_id,
                fleet,
                visibility_kind::owner,
                &oss);

            fleet_t client_fleet = {};
            auto const bytes =
                deserialize_impl(client_fleet, byte_span_of(serialized));
            EXPECT_TRUE(bytes.empty());
            EXPECT_EQ(client_fleet, fleet);
        }
        {
            serialized.clear();
            serialize_for_client(
                gs,
                visible_fleets,
                nation_id,
                fleet,
                visibility_kind::neutral_or_enemy,
                &oss);

            fleet_t client_fleet = {};
            auto const bytes =
                deserialize_impl(client_fleet, byte_span_of(serialized));
            EXPECT_TRUE(bytes.empty());
            EXPECT_NE(client_fleet, fleet);

            EXPECT_EQ(client_fleet.id, fleet.id);

            EXPECT_EQ(client_fleet.units.size(), fleet.units.size());
            EXPECT_EQ(client_fleet.units[0].id, fleet.units[0].id);
            EXPECT_EQ(client_fleet.units[0].health, -1);
            EXPECT_EQ(client_fleet.units[1].id, fleet.units[1].id);
            EXPECT_EQ(client_fleet.units[1].health, -1);
            EXPECT_EQ(client_fleet.units[2].id, fleet.units[2].id);
            EXPECT_EQ(client_fleet.units[2].health, -1);

            EXPECT_EQ(client_fleet.position, fleet.position);

            fleet_t const default_fleet;
            EXPECT_EQ(client_fleet.mission, default_fleet.mission);
            EXPECT_EQ(client_fleet.fuel, default_fleet.fuel);
            EXPECT_EQ(client_fleet.rounds, default_fleet.rounds);
            EXPECT_EQ(client_fleet.missiles, default_fleet.missiles);
            EXPECT_EQ(client_fleet.fighters, default_fleet.fighters);
        }
        {
            serialized.clear();
            serialize_for_client(
                gs,
                visible_fleets,
                nation_id,
                fleet,
                visibility_kind::unseen,
                &oss);
            EXPECT_EQ(byte_span_of(serialized).size(), 1u);
            EXPECT_EQ(byte_span_of(serialized)[0], std::byte(0));
        }
    }

    // hex_t
    {
        std::vector<char> serialized;
        ostream_tarray_facade oss(serialized);
        int const nation_id = 0;
        std::vector<fleet_t const *> visible_fleets;
        hex_t const hex = gs.hexes.front();

        {
            serialized.clear();
            serialize_for_client(
                gs,
                visible_fleets,
                nation_id,
                hex,
                visibility_kind::owner,
                0,
                &oss);

            hex_t client_hex = {};
            auto const bytes =
                deserialize_impl(client_hex, byte_span_of(serialized));
            EXPECT_TRUE(bytes.empty());
            EXPECT_EQ(client_hex, hex);
        }
        {
            serialized.clear();
            serialize_for_client(
                gs,
                visible_fleets,
                nation_id,
                hex,
                visibility_kind::neutral_or_enemy,
                0,
                &oss);

            hex_t client_hex = {};
            auto const bytes =
                deserialize_impl(client_hex, byte_span_of(serialized));
            EXPECT_TRUE(bytes.empty());
            EXPECT_EQ(client_hex, hex);
        }
        {
            serialized.clear();
            serialize_for_client(
                gs,
                visible_fleets,
                nation_id,
                hex,
                visibility_kind::unseen,
                0,
                &oss);
            EXPECT_EQ(byte_span_of(serialized).size(), 1u);
            EXPECT_EQ(byte_span_of(serialized)[0], std::byte(0));
        }
    }

    // system_t
    {
        std::vector<char> serialized;
        ostream_tarray_facade oss(serialized);
        int const nation_id = 0;
        std::vector<fleet_t const *> visible_fleets;
        system_t const system = gs.systems.front();

        {
            serialized.clear();
            serialize_for_client(
                gs,
                visible_fleets,
                nation_id,
                system,
                visibility_kind::owner,
                0,
                &oss);

            system_t client_system = {};
            auto const bytes =
                deserialize_impl(client_system, byte_span_of(serialized));
            EXPECT_TRUE(bytes.empty());
            EXPECT_EQ(client_system, system);
        }
        {
            serialized.clear();
            serialize_for_client(
                gs,
                visible_fleets,
                nation_id,
                system,
                visibility_kind::neutral_or_enemy,
                0,
                &oss);

            system_t client_system = {};
            auto const bytes =
                deserialize_impl(client_system, byte_span_of(serialized));
            EXPECT_TRUE(bytes.empty());
            EXPECT_EQ(client_system, system);
        }
        {
            serialized.clear();
            serialize_for_client(
                gs,
                visible_fleets,
                nation_id,
                system,
                visibility_kind::unseen,
                0,
                &oss);
            EXPECT_EQ(byte_span_of(serialized).size(), 1u);
            EXPECT_EQ(byte_span_of(serialized)[0], std::byte(0));
        }
    }
}

#include <metadata.hpp>

struct fleet_t2
{
    nation_and_object_id_t id = {};
    mission_t mission = mission_t::invalid_mission;
    std::vector<unit_t> units = {};
    float fuel = -1.0f;
    int rounds = -1;
    int missiles = -1;
    int fighters = -1;
    fleet_position_t position = {};
    bool operator==(fleet_t2 const &) const = default;
};

template<>
struct metadata<fleet_t2>
{
    static constexpr std::string_view name()
    {
        using namespace std::literals;
        return "fleet_t2"sv;
    }
    static constexpr int lo_field_number() { return 1; }
    static constexpr int hi_field_number() { return 8; }

    static constexpr metadatum<fleet_t2, nation_and_object_id_t> id()
    {
        using namespace std::literals;
        return {"id"sv, 1, &fleet_t2::id};
    }
    static constexpr metadatum<fleet_t2, mission_t> mission()
    {
        using namespace std::literals;
        return {"mission"sv, 2, &fleet_t2::mission};
    }
    static constexpr metadatum<fleet_t2, std::vector<unit_t>> units()
    {
        using namespace std::literals;
        return {"units"sv, 34, &fleet_t2::units};
    }
    static constexpr metadatum<fleet_t2, float> fuel()
    {
        using namespace std::literals;
        return {"fuel"sv, 4, &fleet_t2::fuel};
    }
    static constexpr metadatum<fleet_t2, int> rounds()
    {
        using namespace std::literals;
        return {"rounds"sv, 5, &fleet_t2::rounds};
    }
    static constexpr metadatum<fleet_t2, int> missiles()
    {
        using namespace std::literals;
        return {"missiles"sv, 6, &fleet_t2::missiles};
    }
    static constexpr metadatum<fleet_t2, int> fighters()
    {
        using namespace std::literals;
        return {"fighters"sv, 7, &fleet_t2::fighters};
    }
    static constexpr metadatum<fleet_t2, fleet_position_t> position()
    {
        using namespace std::literals;
        return {"position"sv, 8, &fleet_t2::position};
    }
};

TEST(client_serialization_tests, TODO)
{
    constexpr auto name = metadata<fleet_t2>::name();

    constexpr auto lo_field_number = metadata<fleet_t2>::lo_field_number();
    constexpr auto hi_field_number = metadata<fleet_t2>::hi_field_number();

    constexpr auto id_meta = metadata<fleet_t2>::id();
    constexpr auto mission_meta = metadata<fleet_t2>::mission();
    constexpr auto units_meta = metadata<fleet_t2>::units();
    constexpr auto fuel_meta = metadata<fleet_t2>::fuel();
    constexpr auto rounds_meta = metadata<fleet_t2>::rounds();
    constexpr auto missiles_meta = metadata<fleet_t2>::missiles();
    constexpr auto fighters_meta = metadata<fleet_t2>::fighters();
    constexpr auto position_meta = metadata<fleet_t2>::position();
}
