// WARNING: Generated code.
// This file was generated from game_data.proto (proto3)
#pragma once

#include <metadata.hpp>

#include "game_data.hpp"
#include <base_types.hpp>
#include <base_types_metadata.hpp>

namespace detail {

    template<> struct metadata<nation_and_object_id_t>
    {
        static constexpr std::string_view struct_name()
        {
            using namespace std::literals;
            return "nation_and_object_id_t"sv;
        }
        static constexpr int lo_field_number() { return 1; }
        static constexpr int hi_field_number() { return 2; }

        static constexpr metadatum<nation_and_object_id_t, int> nation_id()
        {
            using namespace std::literals;
            return {"nation_id"sv, 1, &nation_and_object_id_t::nation_id};
        }
        static constexpr metadatum<nation_and_object_id_t, int> object_id()
        {
            using namespace std::literals;
            return {"object_id"sv, 2, &nation_and_object_id_t::object_id};
        }

        template<typename F>
        static void foreach_member(F && f)
        {
            f(nation_id());
            f(object_id());
        }
    };

    template<> struct metadata<game_start_params_t>
    {
        static constexpr std::string_view struct_name()
        {
            using namespace std::literals;
            return "game_start_params_t"sv;
        }
        static constexpr int lo_field_number() { return 1; }
        static constexpr int hi_field_number() { return 5; }

        static constexpr metadatum<game_start_params_t, float> habitable_systems_per_hex_mean()
        {
            using namespace std::literals;
            return {"habitable_systems_per_hex_mean"sv, 1, &game_start_params_t::habitable_systems_per_hex_mean};
        }
        static constexpr metadatum<game_start_params_t, float> habitable_systems_per_hex_plus_minus()
        {
            using namespace std::literals;
            return {"habitable_systems_per_hex_plus_minus"sv, 2, &game_start_params_t::habitable_systems_per_hex_plus_minus};
        }
        static constexpr metadatum<game_start_params_t, int> systems_per_hex()
        {
            using namespace std::literals;
            return {"systems_per_hex"sv, 3, &game_start_params_t::systems_per_hex};
        }
        static constexpr metadatum<game_start_params_t, int> map_height()
        {
            using namespace std::literals;
            return {"map_height"sv, 4, &game_start_params_t::map_height};
        }
        static constexpr metadatum<game_start_params_t, boost::container::flat_map<int, int>> player_id_to_nation_id()
        {
            using namespace std::literals;
            return {"player_id_to_nation_id"sv, 5, &game_start_params_t::player_id_to_nation_id};
        }

        template<typename F>
        static void foreach_member(F && f)
        {
            f(habitable_systems_per_hex_mean());
            f(habitable_systems_per_hex_plus_minus());
            f(systems_per_hex());
            f(map_height());
            f(player_id_to_nation_id());
        }
    };

    template<> struct metadata<unit_design_t>
    {
        static constexpr std::string_view struct_name()
        {
            using namespace std::literals;
            return "unit_design_t"sv;
        }
        static constexpr int lo_field_number() { return 1; }
        static constexpr int hi_field_number() { return 12; }

        static constexpr metadatum<unit_design_t, nation_and_object_id_t> id()
        {
            using namespace std::literals;
            return {"id"sv, 1, &unit_design_t::id};
        }
        static constexpr metadatum<unit_design_t, int> hull()
        {
            using namespace std::literals;
            return {"hull"sv, 2, &unit_design_t::hull};
        }
        static constexpr metadatum<unit_design_t, int> armor()
        {
            using namespace std::literals;
            return {"armor"sv, 3, &unit_design_t::armor};
        }
        static constexpr metadatum<unit_design_t, int> propulsion()
        {
            using namespace std::literals;
            return {"propulsion"sv, 4, &unit_design_t::propulsion};
        }
        static constexpr metadatum<unit_design_t, int> weapons()
        {
            using namespace std::literals;
            return {"weapons"sv, 5, &unit_design_t::weapons};
        }
        static constexpr metadatum<unit_design_t, int> shields()
        {
            using namespace std::literals;
            return {"shields"sv, 6, &unit_design_t::shields};
        }
        static constexpr metadatum<unit_design_t, int> detection()
        {
            using namespace std::literals;
            return {"detection"sv, 7, &unit_design_t::detection};
        }
        static constexpr metadatum<unit_design_t, int> stealth()
        {
            using namespace std::literals;
            return {"stealth"sv, 8, &unit_design_t::stealth};
        }
        static constexpr metadatum<unit_design_t, int> automation()
        {
            using namespace std::literals;
            return {"automation"sv, 9, &unit_design_t::automation};
        }
        static constexpr metadatum<unit_design_t, int> attack()
        {
            using namespace std::literals;
            return {"attack"sv, 10, &unit_design_t::attack};
        }
        static constexpr metadatum<unit_design_t, int> defense()
        {
            using namespace std::literals;
            return {"defense"sv, 11, &unit_design_t::defense};
        }
        static constexpr metadatum<unit_design_t, int> ground_attack()
        {
            using namespace std::literals;
            return {"ground_attack"sv, 12, &unit_design_t::ground_attack};
        }

        template<typename F>
        static void foreach_member(F && f)
        {
            f(id());
            f(hull());
            f(armor());
            f(propulsion());
            f(weapons());
            f(shields());
            f(detection());
            f(stealth());
            f(automation());
            f(attack());
            f(defense());
            f(ground_attack());
        }
    };

    template<> struct metadata<unit_t>
    {
        static constexpr std::string_view struct_name()
        {
            using namespace std::literals;
            return "unit_t"sv;
        }
        static constexpr int lo_field_number() { return 1; }
        static constexpr int hi_field_number() { return 2; }

        static constexpr metadatum<unit_t, nation_and_object_id_t> id()
        {
            using namespace std::literals;
            return {"id"sv, 1, &unit_t::id};
        }
        static constexpr metadatum<unit_t, int> health()
        {
            using namespace std::literals;
            return {"health"sv, 2, &unit_t::health};
        }

        template<typename F>
        static void foreach_member(F && f)
        {
            f(id());
            f(health());
        }
    };

    template<> struct metadata<fleet_position_t>
    {
        static constexpr std::string_view struct_name()
        {
            using namespace std::literals;
            return "fleet_position_t"sv;
        }
        static constexpr int lo_field_number() { return 1; }
        static constexpr int hi_field_number() { return 7; }

        static constexpr metadatum<fleet_position_t, double> world_pos_x()
        {
            using namespace std::literals;
            return {"world_pos_x"sv, 1, &fleet_position_t::world_pos_x};
        }
        static constexpr metadatum<fleet_position_t, double> world_pos_y()
        {
            using namespace std::literals;
            return {"world_pos_y"sv, 2, &fleet_position_t::world_pos_y};
        }
        static constexpr metadatum<fleet_position_t, int> system_id()
        {
            using namespace std::literals;
            return {"system_id"sv, 3, &fleet_position_t::system_id};
        }
        static constexpr metadatum<fleet_position_t, bool> at_permanent_location()
        {
            using namespace std::literals;
            return {"at_permanent_location"sv, 4, &fleet_position_t::at_permanent_location};
        }
        static constexpr metadatum<fleet_position_t, int> location_index()
        {
            using namespace std::literals;
            return {"location_index"sv, 5, &fleet_position_t::location_index};
        }
        static constexpr metadatum<fleet_position_t, int> object_index()
        {
            using namespace std::literals;
            return {"object_index"sv, 6, &fleet_position_t::object_index};
        }
        static constexpr metadatum<fleet_position_t, bool> is_garrison()
        {
            using namespace std::literals;
            return {"is_garrison"sv, 7, &fleet_position_t::is_garrison};
        }

        template<typename F>
        static void foreach_member(F && f)
        {
            f(world_pos_x());
            f(world_pos_y());
            f(system_id());
            f(at_permanent_location());
            f(location_index());
            f(object_index());
            f(is_garrison());
        }
    };

    template<> struct metadata<fleet_t>
    {
        static constexpr std::string_view struct_name()
        {
            using namespace std::literals;
            return "fleet_t"sv;
        }
        static constexpr int lo_field_number() { return 1; }
        static constexpr int hi_field_number() { return 8; }

        static constexpr metadatum<fleet_t, nation_and_object_id_t> id()
        {
            using namespace std::literals;
            return {"id"sv, 1, &fleet_t::id};
        }
        static constexpr metadatum<fleet_t, mission_t> mission()
        {
            using namespace std::literals;
            return {"mission"sv, 2, &fleet_t::mission};
        }
        static constexpr metadatum<fleet_t, std::vector<unit_t>> units()
        {
            using namespace std::literals;
            return {"units"sv, 3, &fleet_t::units};
        }
        static constexpr metadatum<fleet_t, float> fuel()
        {
            using namespace std::literals;
            return {"fuel"sv, 4, &fleet_t::fuel};
        }
        static constexpr metadatum<fleet_t, int> rounds()
        {
            using namespace std::literals;
            return {"rounds"sv, 5, &fleet_t::rounds};
        }
        static constexpr metadatum<fleet_t, int> missiles()
        {
            using namespace std::literals;
            return {"missiles"sv, 6, &fleet_t::missiles};
        }
        static constexpr metadatum<fleet_t, int> fighters()
        {
            using namespace std::literals;
            return {"fighters"sv, 7, &fleet_t::fighters};
        }
        static constexpr metadatum<fleet_t, fleet_position_t> position()
        {
            using namespace std::literals;
            return {"position"sv, 8, &fleet_t::position};
        }

        template<typename F>
        static void foreach_member(F && f)
        {
            f(id());
            f(mission());
            f(units());
            f(fuel());
            f(rounds());
            f(missiles());
            f(fighters());
            f(position());
        }
    };

    template<> struct metadata<fleets_t>
    {
        static constexpr std::string_view struct_name()
        {
            using namespace std::literals;
            return "fleets_t"sv;
        }
        static constexpr int lo_field_number() { return 1; }
        static constexpr int hi_field_number() { return 1; }

        static constexpr metadatum<fleets_t, boost::container::flat_map<int, int>> fleet_ids()
        {
            using namespace std::literals;
            return {"fleet_ids"sv, 1, &fleets_t::fleet_ids};
        }

        template<typename F>
        static void foreach_member(F && f)
        {
            f(fleet_ids());
        }
    };

    template<> struct metadata<planet_effect_t>
    {
        static constexpr std::string_view struct_name()
        {
            using namespace std::literals;
            return "planet_effect_t"sv;
        }
        static constexpr int lo_field_number() { return 1; }
        static constexpr int hi_field_number() { return 8; }

        static constexpr metadatum<planet_effect_t, adobe::name_t> name()
        {
            using namespace std::literals;
            return {"name"sv, 1, &planet_effect_t::name};
        }
        static constexpr metadatum<planet_effect_t, adobe::name_t> description()
        {
            using namespace std::literals;
            return {"description"sv, 2, &planet_effect_t::description};
        }
        static constexpr metadatum<planet_effect_t, float> amount()
        {
            using namespace std::literals;
            return {"amount"sv, 3, &planet_effect_t::amount};
        }
        static constexpr metadatum<planet_effect_t, int> months_of_effect()
        {
            using namespace std::literals;
            return {"months_of_effect"sv, 4, &planet_effect_t::months_of_effect};
        }
        static constexpr metadatum<planet_effect_t, int> months_remaining()
        {
            using namespace std::literals;
            return {"months_remaining"sv, 5, &planet_effect_t::months_remaining};
        }
        static constexpr metadatum<planet_effect_t, planet_effect_target_t> target()
        {
            using namespace std::literals;
            return {"target"sv, 6, &planet_effect_t::target};
        }
        static constexpr metadatum<planet_effect_t, unsigned int> target_modifiers()
        {
            using namespace std::literals;
            return {"target_modifiers"sv, 7, &planet_effect_t::target_modifiers};
        }
        static constexpr metadatum<planet_effect_t, effect_op_t> operation()
        {
            using namespace std::literals;
            return {"operation"sv, 8, &planet_effect_t::operation};
        }

        template<typename F>
        static void foreach_member(F && f)
        {
            f(name());
            f(description());
            f(amount());
            f(months_of_effect());
            f(months_remaining());
            f(target());
            f(target_modifiers());
            f(operation());
        }
    };

    template<> struct metadata<planet_t>
    {
        static constexpr std::string_view struct_name()
        {
            using namespace std::literals;
            return "planet_t"sv;
        }
        static constexpr int lo_field_number() { return 1; }
        static constexpr int hi_field_number() { return 29; }

        static constexpr metadatum<planet_t, int> system_id()
        {
            using namespace std::literals;
            return {"system_id"sv, 1, &planet_t::system_id};
        }
        static constexpr metadatum<planet_t, planet_type_t> planet_type()
        {
            using namespace std::literals;
            return {"planet_type"sv, 2, &planet_t::planet_type};
        }
        static constexpr metadatum<planet_t, double> mass_kg()
        {
            using namespace std::literals;
            return {"mass_kg"sv, 3, &planet_t::mass_kg};
        }
        static constexpr metadatum<planet_t, double> radius_km()
        {
            using namespace std::literals;
            return {"radius_km"sv, 4, &planet_t::radius_km};
        }
        static constexpr metadatum<planet_t, float> orbit_au()
        {
            using namespace std::literals;
            return {"orbit_au"sv, 5, &planet_t::orbit_au};
        }
        static constexpr metadatum<planet_t, float> orbital_period_y()
        {
            using namespace std::literals;
            return {"orbital_period_y"sv, 6, &planet_t::orbital_period_y};
        }
        static constexpr metadatum<planet_t, float> gravity_g()
        {
            using namespace std::literals;
            return {"gravity_g"sv, 7, &planet_t::gravity_g};
        }
        static constexpr metadatum<planet_t, float> axial_tilt_d()
        {
            using namespace std::literals;
            return {"axial_tilt_d"sv, 8, &planet_t::axial_tilt_d};
        }
        static constexpr metadatum<planet_t, float> day_h()
        {
            using namespace std::literals;
            return {"day_h"sv, 9, &planet_t::day_h};
        }
        static constexpr metadatum<planet_t, float> surface_temperature_k()
        {
            using namespace std::literals;
            return {"surface_temperature_k"sv, 10, &planet_t::surface_temperature_k};
        }
        static constexpr metadatum<planet_t, float> magnetosphere_strength()
        {
            using namespace std::literals;
            return {"magnetosphere_strength"sv, 11, &planet_t::magnetosphere_strength};
        }
        static constexpr metadatum<planet_t, float> atmopsheric_pressure()
        {
            using namespace std::literals;
            return {"atmopsheric_pressure"sv, 12, &planet_t::atmopsheric_pressure};
        }
        static constexpr metadatum<planet_t, float> o2_co2_suitability()
        {
            using namespace std::literals;
            return {"o2_co2_suitability"sv, 13, &planet_t::o2_co2_suitability};
        }
        static constexpr metadatum<planet_t, float> ocean_coverage()
        {
            using namespace std::literals;
            return {"ocean_coverage"sv, 14, &planet_t::ocean_coverage};
        }
        static constexpr metadatum<planet_t, float> growth_factor()
        {
            using namespace std::literals;
            return {"growth_factor"sv, 15, &planet_t::growth_factor};
        }
        static constexpr metadatum<planet_t, atmosphere_type_t> atmosphere_type()
        {
            using namespace std::literals;
            return {"atmosphere_type"sv, 16, &planet_t::atmosphere_type};
        }
        static constexpr metadatum<planet_t, int> water()
        {
            using namespace std::literals;
            return {"water"sv, 17, &planet_t::water};
        }
        static constexpr metadatum<planet_t, int> food()
        {
            using namespace std::literals;
            return {"food"sv, 18, &planet_t::food};
        }
        static constexpr metadatum<planet_t, int> energy()
        {
            using namespace std::literals;
            return {"energy"sv, 19, &planet_t::energy};
        }
        static constexpr metadatum<planet_t, int> metal()
        {
            using namespace std::literals;
            return {"metal"sv, 20, &planet_t::metal};
        }
        static constexpr metadatum<planet_t, int> fuel()
        {
            using namespace std::literals;
            return {"fuel"sv, 21, &planet_t::fuel};
        }
        static constexpr metadatum<planet_t, float> population()
        {
            using namespace std::literals;
            return {"population"sv, 22, &planet_t::population};
        }
        static constexpr metadatum<planet_t, float> infrastructure()
        {
            using namespace std::literals;
            return {"infrastructure"sv, 23, &planet_t::infrastructure};
        }
        static constexpr metadatum<planet_t, float> orbital_pos_r()
        {
            using namespace std::literals;
            return {"orbital_pos_r"sv, 24, &planet_t::orbital_pos_r};
        }
        static constexpr metadatum<planet_t, int> max_population()
        {
            using namespace std::literals;
            return {"max_population"sv, 25, &planet_t::max_population};
        }
        static constexpr metadatum<planet_t, int> owner()
        {
            using namespace std::literals;
            return {"owner"sv, 26, &planet_t::owner};
        }
        static constexpr metadatum<planet_t, int> original_owner()
        {
            using namespace std::literals;
            return {"original_owner"sv, 27, &planet_t::original_owner};
        }
        static constexpr metadatum<planet_t, nation_and_object_id_t> garrison()
        {
            using namespace std::literals;
            return {"garrison"sv, 28, &planet_t::garrison};
        }
        static constexpr metadatum<planet_t, std::vector<planet_effect_t>> effects()
        {
            using namespace std::literals;
            return {"effects"sv, 29, &planet_t::effects};
        }

        template<typename F>
        static void foreach_member(F && f)
        {
            f(system_id());
            f(planet_type());
            f(mass_kg());
            f(radius_km());
            f(orbit_au());
            f(orbital_period_y());
            f(gravity_g());
            f(axial_tilt_d());
            f(day_h());
            f(surface_temperature_k());
            f(magnetosphere_strength());
            f(atmopsheric_pressure());
            f(o2_co2_suitability());
            f(ocean_coverage());
            f(growth_factor());
            f(atmosphere_type());
            f(water());
            f(food());
            f(energy());
            f(metal());
            f(fuel());
            f(population());
            f(infrastructure());
            f(orbital_pos_r());
            f(max_population());
            f(owner());
            f(original_owner());
            f(garrison());
            f(effects());
        }
    };

    template<> struct metadata<location_object_t>
    {
        static constexpr std::string_view struct_name()
        {
            using namespace std::literals;
            return "location_object_t"sv;
        }
        static constexpr int lo_field_number() { return 1; }
        static constexpr int hi_field_number() { return 2; }

        static constexpr metadatum<location_object_t, std::vector<nation_and_object_id_t>> bases()
        {
            using namespace std::literals;
            return {"bases"sv, 1, &location_object_t::bases};
        }
        static constexpr metadatum<location_object_t, std::size_t> planet_id()
        {
            using namespace std::literals;
            return {"planet_id"sv, 2, &location_object_t::planet_id};
        }

        template<typename F>
        static void foreach_member(F && f)
        {
            f(bases());
            f(planet_id());
        }
    };

    template<> struct metadata<system_location_t>
    {
        static constexpr std::string_view struct_name()
        {
            using namespace std::literals;
            return "system_location_t"sv;
        }
        static constexpr int lo_field_number() { return 1; }
        static constexpr int hi_field_number() { return 2; }

        static constexpr metadatum<system_location_t, std::vector<location_object_t>> objects()
        {
            using namespace std::literals;
            return {"objects"sv, 1, &system_location_t::objects};
        }
        static constexpr metadatum<system_location_t, fleets_t> units()
        {
            using namespace std::literals;
            return {"units"sv, 2, &system_location_t::units};
        }

        template<typename F>
        static void foreach_member(F && f)
        {
            f(objects());
            f(units());
        }
    };

    template<> struct metadata<star_t>
    {
        static constexpr std::string_view struct_name()
        {
            using namespace std::literals;
            return "star_t"sv;
        }
        static constexpr int lo_field_number() { return 1; }
        static constexpr int hi_field_number() { return 5; }

        static constexpr metadatum<star_t, star_class_t> star_class()
        {
            using namespace std::literals;
            return {"star_class"sv, 1, &star_t::star_class};
        }
        static constexpr metadatum<star_t, double> temperature_k()
        {
            using namespace std::literals;
            return {"temperature_k"sv, 2, &star_t::temperature_k};
        }
        static constexpr metadatum<star_t, double> solar_masses()
        {
            using namespace std::literals;
            return {"solar_masses"sv, 3, &star_t::solar_masses};
        }
        static constexpr metadatum<star_t, double> solar_luminosities()
        {
            using namespace std::literals;
            return {"solar_luminosities"sv, 4, &star_t::solar_luminosities};
        }
        static constexpr metadatum<star_t, double> solar_radii()
        {
            using namespace std::literals;
            return {"solar_radii"sv, 5, &star_t::solar_radii};
        }

        template<typename F>
        static void foreach_member(F && f)
        {
            f(star_class());
            f(temperature_k());
            f(solar_masses());
            f(solar_luminosities());
            f(solar_radii());
        }
    };

    template<> struct metadata<system_t>
    {
        static constexpr std::string_view struct_name()
        {
            using namespace std::literals;
            return "system_t"sv;
        }
        static constexpr int lo_field_number() { return 1; }
        static constexpr int hi_field_number() { return 9; }

        static constexpr metadatum<system_t, adobe::name_t> name()
        {
            using namespace std::literals;
            return {"name"sv, 1, &system_t::name};
        }
        static constexpr metadatum<system_t, hex_coord_t> coord()
        {
            using namespace std::literals;
            return {"coord"sv, 2, &system_t::coord};
        }
        static constexpr metadatum<system_t, star_t> star()
        {
            using namespace std::literals;
            return {"star"sv, 3, &system_t::star};
        }
        static constexpr metadatum<system_t, std::vector<system_location_t>> permanent_locations()
        {
            using namespace std::literals;
            return {"permanent_locations"sv, 4, &system_t::permanent_locations};
        }
        static constexpr metadatum<system_t, std::vector<system_location_t>> temporary_locations()
        {
            using namespace std::literals;
            return {"temporary_locations"sv, 5, &system_t::temporary_locations};
        }
        static constexpr metadatum<system_t, double> world_pos_x()
        {
            using namespace std::literals;
            return {"world_pos_x"sv, 6, &system_t::world_pos_x};
        }
        static constexpr metadatum<system_t, double> world_pos_y()
        {
            using namespace std::literals;
            return {"world_pos_y"sv, 7, &system_t::world_pos_y};
        }
        static constexpr metadatum<system_t, std::size_t> first_planet()
        {
            using namespace std::literals;
            return {"first_planet"sv, 8, &system_t::first_planet};
        }
        static constexpr metadatum<system_t, std::size_t> last_planet()
        {
            using namespace std::literals;
            return {"last_planet"sv, 9, &system_t::last_planet};
        }

        template<typename F>
        static void foreach_member(F && f)
        {
            f(name());
            f(coord());
            f(star());
            f(permanent_locations());
            f(temporary_locations());
            f(world_pos_x());
            f(world_pos_y());
            f(first_planet());
            f(last_planet());
        }
    };

    template<> struct metadata<hex_t>
    {
        static constexpr std::string_view struct_name()
        {
            using namespace std::literals;
            return "hex_t"sv;
        }
        static constexpr int lo_field_number() { return 1; }
        static constexpr int hi_field_number() { return 4; }

        static constexpr metadatum<hex_t, hex_coord_t> coord()
        {
            using namespace std::literals;
            return {"coord"sv, 1, &hex_t::coord};
        }
        static constexpr metadatum<hex_t, int> province_id()
        {
            using namespace std::literals;
            return {"province_id"sv, 2, &hex_t::province_id};
        }
        static constexpr metadatum<hex_t, std::size_t> first_system()
        {
            using namespace std::literals;
            return {"first_system"sv, 3, &hex_t::first_system};
        }
        static constexpr metadatum<hex_t, std::size_t> last_system()
        {
            using namespace std::literals;
            return {"last_system"sv, 4, &hex_t::last_system};
        }

        template<typename F>
        static void foreach_member(F && f)
        {
            f(coord());
            f(province_id());
            f(first_system());
            f(last_system());
        }
    };

    template<> struct metadata<province_t>
    {
        static constexpr std::string_view struct_name()
        {
            using namespace std::literals;
            return "province_t"sv;
        }
        static constexpr int lo_field_number() { return 1; }
        static constexpr int hi_field_number() { return 2; }

        static constexpr metadatum<province_t, nation_and_object_id_t> id()
        {
            using namespace std::literals;
            return {"id"sv, 1, &province_t::id};
        }
        static constexpr metadatum<province_t, std::vector<hex_coord_t>> hex_coords()
        {
            using namespace std::literals;
            return {"hex_coords"sv, 2, &province_t::hex_coords};
        }

        template<typename F>
        static void foreach_member(F && f)
        {
            f(id());
            f(hex_coords());
        }
    };

    template<> struct metadata<nation_t>
    {
        static constexpr std::string_view struct_name()
        {
            using namespace std::literals;
            return "nation_t"sv;
        }
        static constexpr int lo_field_number() { return 1; }
        static constexpr int hi_field_number() { return 12; }

        static constexpr metadatum<nation_t, int> id()
        {
            using namespace std::literals;
            return {"id"sv, 1, &nation_t::id};
        }
        static constexpr metadatum<nation_t, std::vector<unit_design_t>> unit_designs()
        {
            using namespace std::literals;
            return {"unit_designs"sv, 2, &nation_t::unit_designs};
        }
        static constexpr metadatum<nation_t, std::vector<province_t>> provinces()
        {
            using namespace std::literals;
            return {"provinces"sv, 3, &nation_t::provinces};
        }
        static constexpr metadatum<nation_t, std::vector<fleet_t>> fleets()
        {
            using namespace std::literals;
            return {"fleets"sv, 4, &nation_t::fleets};
        }
        static constexpr metadatum<nation_t, std::vector<int>> hexes_seen()
        {
            using namespace std::literals;
            return {"hexes_seen"sv, 5, &nation_t::hexes_seen};
        }
        static constexpr metadatum<nation_t, std::vector<int>> systems_present_in()
        {
            using namespace std::literals;
            return {"systems_present_in"sv, 6, &nation_t::systems_present_in};
        }
        static constexpr metadatum<nation_t, std::vector<int>> systems_visited()
        {
            using namespace std::literals;
            return {"systems_visited"sv, 7, &nation_t::systems_visited};
        }
        static constexpr metadatum<nation_t, std::vector<int>> planets_present_on()
        {
            using namespace std::literals;
            return {"planets_present_on"sv, 8, &nation_t::planets_present_on};
        }
        static constexpr metadatum<nation_t, std::vector<int>> planets_surveyed()
        {
            using namespace std::literals;
            return {"planets_surveyed"sv, 9, &nation_t::planets_surveyed};
        }
        static constexpr metadatum<nation_t, std::vector<nation_and_object_id_t>> foreign_designs_seen()
        {
            using namespace std::literals;
            return {"foreign_designs_seen"sv, 10, &nation_t::foreign_designs_seen};
        }
        static constexpr metadatum<nation_t, std::vector<nation_and_object_id_t>> foreign_designs_glimpsed()
        {
            using namespace std::literals;
            return {"foreign_designs_glimpsed"sv, 11, &nation_t::foreign_designs_glimpsed};
        }
        static constexpr metadatum<nation_t, bool> defeated()
        {
            using namespace std::literals;
            return {"defeated"sv, 12, &nation_t::defeated};
        }

        template<typename F>
        static void foreach_member(F && f)
        {
            f(id());
            f(unit_designs());
            f(provinces());
            f(fleets());
            f(hexes_seen());
            f(systems_present_in());
            f(systems_visited());
            f(planets_present_on());
            f(planets_surveyed());
            f(foreign_designs_seen());
            f(foreign_designs_glimpsed());
            f(defeated());
        }
    };

    template<> struct metadata<game_state_t>
    {
        static constexpr std::string_view struct_name()
        {
            using namespace std::literals;
            return "game_state_t"sv;
        }
        static constexpr int lo_field_number() { return 1; }
        static constexpr int hi_field_number() { return 8; }

        static constexpr metadatum<game_state_t, int> map_width()
        {
            using namespace std::literals;
            return {"map_width"sv, 1, &game_state_t::map_width};
        }
        static constexpr metadatum<game_state_t, int> map_height()
        {
            using namespace std::literals;
            return {"map_height"sv, 2, &game_state_t::map_height};
        }
        static constexpr metadatum<game_state_t, std::vector<hex_t>> hexes()
        {
            using namespace std::literals;
            return {"hexes"sv, 3, &game_state_t::hexes};
        }
        static constexpr metadatum<game_state_t, std::vector<system_t>> systems()
        {
            using namespace std::literals;
            return {"systems"sv, 4, &game_state_t::systems};
        }
        static constexpr metadatum<game_state_t, std::vector<planet_t>> planets()
        {
            using namespace std::literals;
            return {"planets"sv, 5, &game_state_t::planets};
        }
        static constexpr metadatum<game_state_t, std::vector<nation_t>> nations()
        {
            using namespace std::literals;
            return {"nations"sv, 6, &game_state_t::nations};
        }
        static constexpr metadatum<game_state_t, std::vector<unsigned int>> alliances()
        {
            using namespace std::literals;
            return {"alliances"sv, 7, &game_state_t::alliances};
        }
        static constexpr metadatum<game_state_t, int> play_speed()
        {
            using namespace std::literals;
            return {"play_speed"sv, 8, &game_state_t::play_speed};
        }

        template<typename F>
        static void foreach_member(F && f)
        {
            f(map_width());
            f(map_height());
            f(hexes());
            f(systems());
            f(planets());
            f(nations());
            f(alliances());
            f(play_speed());
        }
    };

}


