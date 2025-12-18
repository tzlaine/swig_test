#include <game_data_metadata.hpp>
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

game_state_t const & gs()
{
    static game_state_t retval = [] {
        task_system ts(4);
        game_state_t retval;
        std::atomic_bool fully_complete = false;
        generation::generate_galaxy(
            default_game_start_params(), retval, &ts, nullptr, &fully_complete);
        while (!fully_complete) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        retval.nations = {
            nation(0, retval), nation(1, retval), nation(2, retval)};
        EXPECT_FALSE(retval.hexes.empty());
        EXPECT_FALSE(retval.systems.empty());
        EXPECT_FALSE(retval.planets.empty());
        return retval;
    }();
    return retval;
}

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
                gs(),
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
                gs(),
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
                gs(),
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
                gs(),
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
                gs(),
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
                gs(),
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
                gs(),
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
                gs(),
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
                gs(),
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
        hex_t const hex = gs().hexes.front();

        {
            serialized.clear();
            serialize_for_client(
                gs(),
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
                gs(),
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
                gs(),
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
        system_t const system = gs().systems.front();

        {
            serialized.clear();
            serialize_for_client(
                gs(),
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
                gs(),
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

            system_t const default_system;
            EXPECT_EQ(client_system.name, system.name);
            EXPECT_EQ(client_system.coord, system.coord);
            EXPECT_EQ(client_system.star, system.star);
            EXPECT_TRUE(client_system.permanent_locations.empty());
            EXPECT_TRUE(client_system.temporary_locations.empty());
            EXPECT_EQ(client_system.world_pos_x, system.world_pos_x);
            EXPECT_EQ(client_system.world_pos_y, system.world_pos_y);
            EXPECT_EQ(client_system.first_planet, default_system.first_planet);
            EXPECT_EQ(client_system.last_planet, default_system.last_planet);
        }
        {
            serialized.clear();
            serialize_for_client(
                gs(),
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

    // planet_t
    {
        std::vector<char> serialized;
        ostream_tarray_facade oss(serialized);
        int const nation_id = 0;
        std::vector<fleet_t const *> visible_fleets;
        planet_t const planet = gs().planets.front();

        {
            serialized.clear();
            serialize_for_client(
                gs(),
                visible_fleets,
                nation_id,
                planet,
                visibility_kind::owner,
                0,
                &oss);

            planet_t client_planet = {};
            auto const bytes =
                deserialize_impl(client_planet, byte_span_of(serialized));
            EXPECT_TRUE(bytes.empty());
            EXPECT_EQ(client_planet, planet);
        }
        {
            serialized.clear();
            serialize_for_client(
                gs(),
                visible_fleets,
                nation_id,
                planet,
                visibility_kind::neutral_or_enemy,
                0,
                &oss);

            planet_t client_planet = {};
            auto const bytes =
                deserialize_impl(client_planet, byte_span_of(serialized));
            EXPECT_TRUE(bytes.empty());

            planet_t const default_planet;
            EXPECT_EQ(client_planet.system_id, planet.system_id);
            EXPECT_EQ(client_planet.planet_type, planet.planet_type);
            EXPECT_EQ(client_planet.mass_kg, planet.mass_kg);
            EXPECT_EQ(client_planet.radius_km, planet.radius_km);
            EXPECT_EQ(client_planet.orbit_au, planet.orbit_au);
            EXPECT_EQ(client_planet.orbital_period_y, planet.orbital_period_y);
            EXPECT_EQ(client_planet.gravity_g, planet.gravity_g);
            EXPECT_EQ(client_planet.axial_tilt_d, planet.axial_tilt_d);
            EXPECT_EQ(client_planet.day_h, planet.day_h);
            EXPECT_EQ(
                client_planet.surface_temperature_k,
                planet.surface_temperature_k);
            EXPECT_EQ(
                client_planet.magnetosphere_strength,
                planet.magnetosphere_strength);
            EXPECT_EQ(
                client_planet.atmopsheric_pressure,
                planet.atmopsheric_pressure);
            EXPECT_EQ(
                client_planet.o2_co2_suitability, planet.o2_co2_suitability);
            EXPECT_EQ(client_planet.ocean_coverage, planet.ocean_coverage);
            EXPECT_EQ(client_planet.growth_factor, planet.growth_factor);
            EXPECT_EQ(client_planet.atmosphere_type, planet.atmosphere_type);
            EXPECT_EQ(client_planet.water, default_planet.water);
            EXPECT_EQ(client_planet.food, default_planet.food);
            EXPECT_EQ(client_planet.energy, default_planet.energy);
            EXPECT_EQ(client_planet.metal, default_planet.metal);
            EXPECT_EQ(client_planet.fuel, default_planet.fuel);
            EXPECT_EQ(client_planet.population, default_planet.population);
            EXPECT_EQ(
                client_planet.infrastructure, default_planet.infrastructure);
            EXPECT_EQ(client_planet.orbital_pos_r, planet.orbital_pos_r);
            EXPECT_EQ(client_planet.max_population, planet.max_population);
            EXPECT_EQ(client_planet.owner, planet.owner);
            EXPECT_EQ(client_planet.original_owner, planet.original_owner);
            EXPECT_EQ(client_planet.garrison, default_planet.garrison);
            EXPECT_EQ(client_planet.effects, planet.effects);
        }
        {
            serialized.clear();
            serialize_for_client(
                gs(),
                visible_fleets,
                nation_id,
                planet,
                visibility_kind::unseen,
                0,
                &oss);
            EXPECT_EQ(byte_span_of(serialized).size(), 1u);
            EXPECT_EQ(byte_span_of(serialized)[0], std::byte(0));
        }
    }
}

TEST(client_serialization_tests, metadata)
{
    constexpr auto name = detail::metadata<fleet_t>::struct_name();
    EXPECT_EQ(name, "fleet_t");

    constexpr int lo_field_number =
        detail::metadata<fleet_t>::lo_field_number();
    EXPECT_EQ(lo_field_number, 1);
    constexpr int hi_field_number =
        detail::metadata<fleet_t>::hi_field_number();

    constexpr auto id_meta = detail::metadata<fleet_t>::id();
    EXPECT_EQ(id_meta.name_, "id");
    EXPECT_EQ(id_meta.ptr_, &fleet_t::id);
    constexpr auto mission_meta = detail::metadata<fleet_t>::mission();
    EXPECT_EQ(mission_meta.name_, "mission");
    EXPECT_EQ(mission_meta.ptr_, &fleet_t::mission);
    constexpr auto units_meta = detail::metadata<fleet_t>::units();
    EXPECT_EQ(units_meta.name_, "units");
    EXPECT_EQ(units_meta.ptr_, &fleet_t::units);
    constexpr auto fuel_meta = detail::metadata<fleet_t>::fuel();
    EXPECT_EQ(fuel_meta.name_, "fuel");
    EXPECT_EQ(fuel_meta.ptr_, &fleet_t::fuel);
    constexpr auto rounds_meta = detail::metadata<fleet_t>::rounds();
    EXPECT_EQ(rounds_meta.name_, "rounds");
    EXPECT_EQ(rounds_meta.ptr_, &fleet_t::rounds);
    constexpr auto missiles_meta = detail::metadata<fleet_t>::missiles();
    EXPECT_EQ(missiles_meta.name_, "missiles");
    EXPECT_EQ(missiles_meta.ptr_, &fleet_t::missiles);
    constexpr auto fighters_meta = detail::metadata<fleet_t>::fighters();
    EXPECT_EQ(fighters_meta.name_, "fighters");
    EXPECT_EQ(fighters_meta.ptr_, &fleet_t::fighters);
    constexpr auto position_meta = detail::metadata<fleet_t>::position();
    EXPECT_EQ(position_meta.name_, "position");
    EXPECT_EQ(position_meta.ptr_, &fleet_t::position);
}
