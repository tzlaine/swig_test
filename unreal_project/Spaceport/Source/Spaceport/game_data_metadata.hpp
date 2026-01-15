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
        static constexpr int hi_field_number() { return 6; }

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
        static constexpr metadatum<game_start_params_t, int> ai_opponents()
        {
            using namespace std::literals;
            return {"ai_opponents"sv, 6, &game_start_params_t::ai_opponents};
        }

        template<typename F>
        static void foreach_member(F && f)
        {
            f(habitable_systems_per_hex_mean());
            f(habitable_systems_per_hex_plus_minus());
            f(systems_per_hex());
            f(map_height());
            f(player_id_to_nation_id());
            f(ai_opponents());
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
        static constexpr int hi_field_number() { return 102; }

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
        static constexpr metadatum<unit_design_t, int> fuel()
        {
            using namespace std::literals;
            return {"fuel"sv, 9, &unit_design_t::fuel};
        }
        static constexpr metadatum<unit_design_t, int> water()
        {
            using namespace std::literals;
            return {"water"sv, 10, &unit_design_t::water};
        }
        static constexpr metadatum<unit_design_t, int> supplies()
        {
            using namespace std::literals;
            return {"supplies"sv, 11, &unit_design_t::supplies};
        }
        static constexpr metadatum<unit_design_t, int> rounds()
        {
            using namespace std::literals;
            return {"rounds"sv, 12, &unit_design_t::rounds};
        }
        static constexpr metadatum<unit_design_t, int> missiles()
        {
            using namespace std::literals;
            return {"missiles"sv, 13, &unit_design_t::missiles};
        }
        static constexpr metadatum<unit_design_t, int> fighters()
        {
            using namespace std::literals;
            return {"fighters"sv, 14, &unit_design_t::fighters};
        }
        static constexpr metadatum<unit_design_t, int> cargo()
        {
            using namespace std::literals;
            return {"cargo"sv, 15, &unit_design_t::cargo};
        }
        static constexpr metadatum<unit_design_t, int> propulsion_space()
        {
            using namespace std::literals;
            return {"propulsion_space"sv, 16, &unit_design_t::propulsion_space};
        }
        static constexpr metadatum<unit_design_t, int> weapons_space()
        {
            using namespace std::literals;
            return {"weapons_space"sv, 17, &unit_design_t::weapons_space};
        }
        static constexpr metadatum<unit_design_t, int> shields_space()
        {
            using namespace std::literals;
            return {"shields_space"sv, 18, &unit_design_t::shields_space};
        }
        static constexpr metadatum<unit_design_t, int> detection_space()
        {
            using namespace std::literals;
            return {"detection_space"sv, 19, &unit_design_t::detection_space};
        }
        static constexpr metadatum<unit_design_t, int> stealth_space()
        {
            using namespace std::literals;
            return {"stealth_space"sv, 20, &unit_design_t::stealth_space};
        }
        static constexpr metadatum<unit_design_t, float> effective_propulsion()
        {
            using namespace std::literals;
            return {"effective_propulsion"sv, 21, &unit_design_t::effective_propulsion};
        }
        static constexpr metadatum<unit_design_t, float> effective_weapons()
        {
            using namespace std::literals;
            return {"effective_weapons"sv, 22, &unit_design_t::effective_weapons};
        }
        static constexpr metadatum<unit_design_t, float> effective_shields()
        {
            using namespace std::literals;
            return {"effective_shields"sv, 23, &unit_design_t::effective_shields};
        }
        static constexpr metadatum<unit_design_t, float> effective_detection()
        {
            using namespace std::literals;
            return {"effective_detection"sv, 24, &unit_design_t::effective_detection};
        }
        static constexpr metadatum<unit_design_t, float> effective_stealth()
        {
            using namespace std::literals;
            return {"effective_stealth"sv, 25, &unit_design_t::effective_stealth};
        }
        static constexpr metadatum<unit_design_t, int> propulsion_reliability()
        {
            using namespace std::literals;
            return {"propulsion_reliability"sv, 26, &unit_design_t::propulsion_reliability};
        }
        static constexpr metadatum<unit_design_t, int> weapons_reliability()
        {
            using namespace std::literals;
            return {"weapons_reliability"sv, 27, &unit_design_t::weapons_reliability};
        }
        static constexpr metadatum<unit_design_t, int> shields_reliability()
        {
            using namespace std::literals;
            return {"shields_reliability"sv, 28, &unit_design_t::shields_reliability};
        }
        static constexpr metadatum<unit_design_t, int> detection_reliability()
        {
            using namespace std::literals;
            return {"detection_reliability"sv, 29, &unit_design_t::detection_reliability};
        }
        static constexpr metadatum<unit_design_t, int> stealth_reliability()
        {
            using namespace std::literals;
            return {"stealth_reliability"sv, 30, &unit_design_t::stealth_reliability};
        }
        static constexpr metadatum<unit_design_t, int> known_propulsion_failures()
        {
            using namespace std::literals;
            return {"known_propulsion_failures"sv, 31, &unit_design_t::known_propulsion_failures};
        }
        static constexpr metadatum<unit_design_t, int> known_weapons_failures()
        {
            using namespace std::literals;
            return {"known_weapons_failures"sv, 32, &unit_design_t::known_weapons_failures};
        }
        static constexpr metadatum<unit_design_t, int> known_shields_failures()
        {
            using namespace std::literals;
            return {"known_shields_failures"sv, 33, &unit_design_t::known_shields_failures};
        }
        static constexpr metadatum<unit_design_t, int> known_detection_failures()
        {
            using namespace std::literals;
            return {"known_detection_failures"sv, 34, &unit_design_t::known_detection_failures};
        }
        static constexpr metadatum<unit_design_t, int> known_stealth_failures()
        {
            using namespace std::literals;
            return {"known_stealth_failures"sv, 35, &unit_design_t::known_stealth_failures};
        }
        static constexpr metadatum<unit_design_t, float> design_months_remaining()
        {
            using namespace std::literals;
            return {"design_months_remaining"sv, 36, &unit_design_t::design_months_remaining};
        }
        static constexpr metadatum<unit_design_t, std::vector<signed char>> hit_table()
        {
            using namespace std::literals;
            return {"hit_table"sv, 100, &unit_design_t::hit_table};
        }
        static constexpr metadatum<unit_design_t, bool> failed_design()
        {
            using namespace std::literals;
            return {"failed_design"sv, 101, &unit_design_t::failed_design};
        }
        static constexpr metadatum<unit_design_t, bool> obsolete()
        {
            using namespace std::literals;
            return {"obsolete"sv, 102, &unit_design_t::obsolete};
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
            f(fuel());
            f(water());
            f(supplies());
            f(rounds());
            f(missiles());
            f(fighters());
            f(cargo());
            f(propulsion_space());
            f(weapons_space());
            f(shields_space());
            f(detection_space());
            f(stealth_space());
            f(effective_propulsion());
            f(effective_weapons());
            f(effective_shields());
            f(effective_detection());
            f(effective_stealth());
            f(propulsion_reliability());
            f(weapons_reliability());
            f(shields_reliability());
            f(detection_reliability());
            f(stealth_reliability());
            f(known_propulsion_failures());
            f(known_weapons_failures());
            f(known_shields_failures());
            f(known_detection_failures());
            f(known_stealth_failures());
            f(design_months_remaining());
            f(hit_table());
            f(failed_design());
            f(obsolete());
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
        static constexpr int hi_field_number() { return 19; }

        static constexpr metadatum<unit_t, nation_and_object_id_t> id()
        {
            using namespace std::literals;
            return {"id"sv, 1, &unit_t::id};
        }
        static constexpr metadatum<unit_t, int> hull()
        {
            using namespace std::literals;
            return {"hull"sv, 2, &unit_t::hull};
        }
        static constexpr metadatum<unit_t, int> armor()
        {
            using namespace std::literals;
            return {"armor"sv, 3, &unit_t::armor};
        }
        static constexpr metadatum<unit_t, float> propulsion()
        {
            using namespace std::literals;
            return {"propulsion"sv, 4, &unit_t::propulsion};
        }
        static constexpr metadatum<unit_t, float> weapons()
        {
            using namespace std::literals;
            return {"weapons"sv, 5, &unit_t::weapons};
        }
        static constexpr metadatum<unit_t, float> shields()
        {
            using namespace std::literals;
            return {"shields"sv, 6, &unit_t::shields};
        }
        static constexpr metadatum<unit_t, float> detection()
        {
            using namespace std::literals;
            return {"detection"sv, 7, &unit_t::detection};
        }
        static constexpr metadatum<unit_t, float> stealth()
        {
            using namespace std::literals;
            return {"stealth"sv, 8, &unit_t::stealth};
        }
        static constexpr metadatum<unit_t, float> fuel()
        {
            using namespace std::literals;
            return {"fuel"sv, 9, &unit_t::fuel};
        }
        static constexpr metadatum<unit_t, float> water()
        {
            using namespace std::literals;
            return {"water"sv, 10, &unit_t::water};
        }
        static constexpr metadatum<unit_t, float> supplies()
        {
            using namespace std::literals;
            return {"supplies"sv, 11, &unit_t::supplies};
        }
        static constexpr metadatum<unit_t, int> rounds()
        {
            using namespace std::literals;
            return {"rounds"sv, 12, &unit_t::rounds};
        }
        static constexpr metadatum<unit_t, int> missiles()
        {
            using namespace std::literals;
            return {"missiles"sv, 13, &unit_t::missiles};
        }
        static constexpr metadatum<unit_t, int> fighters()
        {
            using namespace std::literals;
            return {"fighters"sv, 14, &unit_t::fighters};
        }
        static constexpr metadatum<unit_t, std::vector<signed char>> cargo()
        {
            using namespace std::literals;
            return {"cargo"sv, 15, &unit_t::cargo};
        }
        static constexpr metadatum<unit_t, int> organization()
        {
            using namespace std::literals;
            return {"organization"sv, 16, &unit_t::organization};
        }
        static constexpr metadatum<unit_t, int> experience()
        {
            using namespace std::literals;
            return {"experience"sv, 17, &unit_t::experience};
        }
        static constexpr metadatum<unit_t, int> crew()
        {
            using namespace std::literals;
            return {"crew"sv, 18, &unit_t::crew};
        }
        static constexpr metadatum<unit_t, std::vector<signed char>> hit_table()
        {
            using namespace std::literals;
            return {"hit_table"sv, 19, &unit_t::hit_table};
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
            f(fuel());
            f(water());
            f(supplies());
            f(rounds());
            f(missiles());
            f(fighters());
            f(cargo());
            f(organization());
            f(experience());
            f(crew());
            f(hit_table());
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
        static constexpr int hi_field_number() { return 5; }

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
        static constexpr metadatum<fleet_t, fleet_position_t> position()
        {
            using namespace std::literals;
            return {"position"sv, 4, &fleet_t::position};
        }
        static constexpr metadatum<fleet_t, int> fleet_experience()
        {
            using namespace std::literals;
            return {"fleet_experience"sv, 5, &fleet_t::fleet_experience};
        }

        template<typename F>
        static void foreach_member(F && f)
        {
            f(id());
            f(mission());
            f(units());
            f(position());
            f(fleet_experience());
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

    template<> struct metadata<resource_t>
    {
        static constexpr std::string_view struct_name()
        {
            using namespace std::literals;
            return "resource_t"sv;
        }
        static constexpr int lo_field_number() { return 1; }
        static constexpr int hi_field_number() { return 5; }

        static constexpr metadatum<resource_t, int> availability()
        {
            using namespace std::literals;
            return {"availability"sv, 1, &resource_t::availability};
        }
        static constexpr metadatum<resource_t, int> max_availability()
        {
            using namespace std::literals;
            return {"max_availability"sv, 2, &resource_t::max_availability};
        }
        static constexpr metadatum<resource_t, float> stockpile()
        {
            using namespace std::literals;
            return {"stockpile"sv, 3, &resource_t::stockpile};
        }
        static constexpr metadatum<resource_t, float> max_stockpile()
        {
            using namespace std::literals;
            return {"max_stockpile"sv, 4, &resource_t::max_stockpile};
        }
        static constexpr metadatum<resource_t, float> stockpile_last_month()
        {
            using namespace std::literals;
            return {"stockpile_last_month"sv, 5, &resource_t::stockpile_last_month};
        }

        template<typename F>
        static void foreach_member(F && f)
        {
            f(availability());
            f(max_availability());
            f(stockpile());
            f(max_stockpile());
            f(stockpile_last_month());
        }
    };

    template<> struct metadata<settlement_t>
    {
        static constexpr std::string_view struct_name()
        {
            using namespace std::literals;
            return "settlement_t"sv;
        }
        static constexpr int lo_field_number() { return 1; }
        static constexpr int hi_field_number() { return 25; }

        static constexpr metadatum<settlement_t, nation_and_object_id_t> id()
        {
            using namespace std::literals;
            return {"id"sv, 1, &settlement_t::id};
        }
        static constexpr metadatum<settlement_t, int> planet_id()
        {
            using namespace std::literals;
            return {"planet_id"sv, 2, &settlement_t::planet_id};
        }
        static constexpr metadatum<settlement_t, int> original_owner()
        {
            using namespace std::literals;
            return {"original_owner"sv, 3, &settlement_t::original_owner};
        }
        static constexpr metadatum<settlement_t, float> population()
        {
            using namespace std::literals;
            return {"population"sv, 4, &settlement_t::population};
        }
        static constexpr metadatum<settlement_t, float> infrastructure()
        {
            using namespace std::literals;
            return {"infrastructure"sv, 5, &settlement_t::infrastructure};
        }
        static constexpr metadatum<settlement_t, resource_t> water()
        {
            using namespace std::literals;
            return {"water"sv, 6, &settlement_t::water};
        }
        static constexpr metadatum<settlement_t, resource_t> food()
        {
            using namespace std::literals;
            return {"food"sv, 7, &settlement_t::food};
        }
        static constexpr metadatum<settlement_t, resource_t> energy()
        {
            using namespace std::literals;
            return {"energy"sv, 8, &settlement_t::energy};
        }
        static constexpr metadatum<settlement_t, resource_t> metal()
        {
            using namespace std::literals;
            return {"metal"sv, 9, &settlement_t::metal};
        }
        static constexpr metadatum<settlement_t, resource_t> fuel_minerals()
        {
            using namespace std::literals;
            return {"fuel_minerals"sv, 10, &settlement_t::fuel_minerals};
        }
        static constexpr metadatum<settlement_t, int> shipyard_pops()
        {
            using namespace std::literals;
            return {"shipyard_pops"sv, 11, &settlement_t::shipyard_pops};
        }
        static constexpr metadatum<settlement_t, int> infrastructure_upgrade_pops()
        {
            using namespace std::literals;
            return {"infrastructure_upgrade_pops"sv, 12, &settlement_t::infrastructure_upgrade_pops};
        }
        static constexpr metadatum<settlement_t, int> infrastructure_maintenance_pops()
        {
            using namespace std::literals;
            return {"infrastructure_maintenance_pops"sv, 13, &settlement_t::infrastructure_maintenance_pops};
        }
        static constexpr metadatum<settlement_t, int> infrastructure_repair_pops()
        {
            using namespace std::literals;
            return {"infrastructure_repair_pops"sv, 14, &settlement_t::infrastructure_repair_pops};
        }
        static constexpr metadatum<settlement_t, int> water_pops()
        {
            using namespace std::literals;
            return {"water_pops"sv, 15, &settlement_t::water_pops};
        }
        static constexpr metadatum<settlement_t, int> food_pops()
        {
            using namespace std::literals;
            return {"food_pops"sv, 16, &settlement_t::food_pops};
        }
        static constexpr metadatum<settlement_t, int> energy_pops()
        {
            using namespace std::literals;
            return {"energy_pops"sv, 17, &settlement_t::energy_pops};
        }
        static constexpr metadatum<settlement_t, int> metal_pops()
        {
            using namespace std::literals;
            return {"metal_pops"sv, 18, &settlement_t::metal_pops};
        }
        static constexpr metadatum<settlement_t, int> fuel_minerals_pops()
        {
            using namespace std::literals;
            return {"fuel_minerals_pops"sv, 19, &settlement_t::fuel_minerals_pops};
        }
        static constexpr metadatum<settlement_t, int> fuel_refining_pops()
        {
            using namespace std::literals;
            return {"fuel_refining_pops"sv, 20, &settlement_t::fuel_refining_pops};
        }
        static constexpr metadatum<settlement_t, int> supply_manufaturing_pops()
        {
            using namespace std::literals;
            return {"supply_manufaturing_pops"sv, 21, &settlement_t::supply_manufaturing_pops};
        }
        static constexpr metadatum<settlement_t, int> round_manufaturing_pops()
        {
            using namespace std::literals;
            return {"round_manufaturing_pops"sv, 22, &settlement_t::round_manufaturing_pops};
        }
        static constexpr metadatum<settlement_t, int> missile_manufaturing_pops()
        {
            using namespace std::literals;
            return {"missile_manufaturing_pops"sv, 23, &settlement_t::missile_manufaturing_pops};
        }
        static constexpr metadatum<settlement_t, int> fighter_manufaturing_pops()
        {
            using namespace std::literals;
            return {"fighter_manufaturing_pops"sv, 24, &settlement_t::fighter_manufaturing_pops};
        }
        static constexpr metadatum<settlement_t, std::vector<nation_and_object_id_t>> garrison()
        {
            using namespace std::literals;
            return {"garrison"sv, 25, &settlement_t::garrison};
        }

        template<typename F>
        static void foreach_member(F && f)
        {
            f(id());
            f(planet_id());
            f(original_owner());
            f(population());
            f(infrastructure());
            f(water());
            f(food());
            f(energy());
            f(metal());
            f(fuel_minerals());
            f(shipyard_pops());
            f(infrastructure_upgrade_pops());
            f(infrastructure_maintenance_pops());
            f(infrastructure_repair_pops());
            f(water_pops());
            f(food_pops());
            f(energy_pops());
            f(metal_pops());
            f(fuel_minerals_pops());
            f(fuel_refining_pops());
            f(supply_manufaturing_pops());
            f(round_manufaturing_pops());
            f(missile_manufaturing_pops());
            f(fighter_manufaturing_pops());
            f(garrison());
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
        static constexpr int hi_field_number() { return 3; }

        static constexpr metadatum<planet_effect_t, adobe::name_t> name()
        {
            using namespace std::literals;
            return {"name"sv, 1, &planet_effect_t::name};
        }
        static constexpr metadatum<planet_effect_t, adobe::name_t> reason()
        {
            using namespace std::literals;
            return {"reason"sv, 2, &planet_effect_t::reason};
        }
        static constexpr metadatum<planet_effect_t, float> value()
        {
            using namespace std::literals;
            return {"value"sv, 3, &planet_effect_t::value};
        }

        template<typename F>
        static void foreach_member(F && f)
        {
            f(name());
            f(reason());
            f(value());
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
        static constexpr int hi_field_number() { return 26; }

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
        static constexpr metadatum<planet_t, float> atmospheric_pressure()
        {
            using namespace std::literals;
            return {"atmospheric_pressure"sv, 12, &planet_t::atmospheric_pressure};
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
        static constexpr metadatum<planet_t, int> fuel_minerals()
        {
            using namespace std::literals;
            return {"fuel_minerals"sv, 21, &planet_t::fuel_minerals};
        }
        static constexpr metadatum<planet_t, float> infrastructure_cost_factor()
        {
            using namespace std::literals;
            return {"infrastructure_cost_factor"sv, 22, &planet_t::infrastructure_cost_factor};
        }
        static constexpr metadatum<planet_t, float> orbital_pos_r()
        {
            using namespace std::literals;
            return {"orbital_pos_r"sv, 23, &planet_t::orbital_pos_r};
        }
        static constexpr metadatum<planet_t, int> max_population()
        {
            using namespace std::literals;
            return {"max_population"sv, 24, &planet_t::max_population};
        }
        static constexpr metadatum<planet_t, std::vector<planet_effect_t>> effects()
        {
            using namespace std::literals;
            return {"effects"sv, 25, &planet_t::effects};
        }
        static constexpr metadatum<planet_t, std::vector<nation_and_object_id_t>> settlement_ids()
        {
            using namespace std::literals;
            return {"settlement_ids"sv, 26, &planet_t::settlement_ids};
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
            f(atmospheric_pressure());
            f(o2_co2_suitability());
            f(ocean_coverage());
            f(growth_factor());
            f(atmosphere_type());
            f(water());
            f(food());
            f(energy());
            f(metal());
            f(fuel_minerals());
            f(infrastructure_cost_factor());
            f(orbital_pos_r());
            f(max_population());
            f(effects());
            f(settlement_ids());
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
        static constexpr metadatum<system_t, int> hex_id()
        {
            using namespace std::literals;
            return {"hex_id"sv, 2, &system_t::hex_id};
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
            f(hex_id());
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
        static constexpr int hi_field_number() { return 100; }

        static constexpr metadatum<nation_t, int> id()
        {
            using namespace std::literals;
            return {"id"sv, 1, &nation_t::id};
        }
        static constexpr metadatum<nation_t, int> home_planet()
        {
            using namespace std::literals;
            return {"home_planet"sv, 2, &nation_t::home_planet};
        }
        static constexpr metadatum<nation_t, nation_and_object_id_t> capitol_settlement()
        {
            using namespace std::literals;
            return {"capitol_settlement"sv, 3, &nation_t::capitol_settlement};
        }
        static constexpr metadatum<nation_t, int> money()
        {
            using namespace std::literals;
            return {"money"sv, 4, &nation_t::money};
        }
        static constexpr metadatum<nation_t, std::vector<unit_design_t>> unit_designs()
        {
            using namespace std::literals;
            return {"unit_designs"sv, 5, &nation_t::unit_designs};
        }
        static constexpr metadatum<nation_t, std::vector<province_t>> provinces()
        {
            using namespace std::literals;
            return {"provinces"sv, 6, &nation_t::provinces};
        }
        static constexpr metadatum<nation_t, std::vector<settlement_t>> settlements()
        {
            using namespace std::literals;
            return {"settlements"sv, 7, &nation_t::settlements};
        }
        static constexpr metadatum<nation_t, std::vector<fleet_t>> fleets()
        {
            using namespace std::literals;
            return {"fleets"sv, 8, &nation_t::fleets};
        }
        static constexpr metadatum<nation_t, int> transports()
        {
            using namespace std::literals;
            return {"transports"sv, 9, &nation_t::transports};
        }
        static constexpr metadatum<nation_t, std::vector<int>> hexes_seen()
        {
            using namespace std::literals;
            return {"hexes_seen"sv, 10, &nation_t::hexes_seen};
        }
        static constexpr metadatum<nation_t, std::vector<int>> systems_present_in()
        {
            using namespace std::literals;
            return {"systems_present_in"sv, 11, &nation_t::systems_present_in};
        }
        static constexpr metadatum<nation_t, std::vector<int>> systems_visited()
        {
            using namespace std::literals;
            return {"systems_visited"sv, 12, &nation_t::systems_visited};
        }
        static constexpr metadatum<nation_t, std::vector<int>> planets_surveyed()
        {
            using namespace std::literals;
            return {"planets_surveyed"sv, 13, &nation_t::planets_surveyed};
        }
        static constexpr metadatum<nation_t, std::vector<settlement_t>> settlements_seen()
        {
            using namespace std::literals;
            return {"settlements_seen"sv, 14, &nation_t::settlements_seen};
        }
        static constexpr metadatum<nation_t, std::vector<nation_and_object_id_t>> foreign_designs_seen()
        {
            using namespace std::literals;
            return {"foreign_designs_seen"sv, 15, &nation_t::foreign_designs_seen};
        }
        static constexpr metadatum<nation_t, std::vector<nation_and_object_id_t>> foreign_designs_glimpsed()
        {
            using namespace std::literals;
            return {"foreign_designs_glimpsed"sv, 16, &nation_t::foreign_designs_glimpsed};
        }
        static constexpr metadatum<nation_t, float> construction_tech()
        {
            using namespace std::literals;
            return {"construction_tech"sv, 17, &nation_t::construction_tech};
        }
        static constexpr metadatum<nation_t, float> propulsion_tech()
        {
            using namespace std::literals;
            return {"propulsion_tech"sv, 18, &nation_t::propulsion_tech};
        }
        static constexpr metadatum<nation_t, float> weapons_tech()
        {
            using namespace std::literals;
            return {"weapons_tech"sv, 19, &nation_t::weapons_tech};
        }
        static constexpr metadatum<nation_t, float> shields_tech()
        {
            using namespace std::literals;
            return {"shields_tech"sv, 20, &nation_t::shields_tech};
        }
        static constexpr metadatum<nation_t, float> stealth_tech()
        {
            using namespace std::literals;
            return {"stealth_tech"sv, 21, &nation_t::stealth_tech};
        }
        static constexpr metadatum<nation_t, float> detection_tech()
        {
            using namespace std::literals;
            return {"detection_tech"sv, 22, &nation_t::detection_tech};
        }
        static constexpr metadatum<nation_t, float> automation_tech()
        {
            using namespace std::literals;
            return {"automation_tech"sv, 23, &nation_t::automation_tech};
        }
        static constexpr metadatum<nation_t, int> construction_tradition()
        {
            using namespace std::literals;
            return {"construction_tradition"sv, 24, &nation_t::construction_tradition};
        }
        static constexpr metadatum<nation_t, int> propulsion_tradition()
        {
            using namespace std::literals;
            return {"propulsion_tradition"sv, 25, &nation_t::propulsion_tradition};
        }
        static constexpr metadatum<nation_t, int> weapons_tradition()
        {
            using namespace std::literals;
            return {"weapons_tradition"sv, 26, &nation_t::weapons_tradition};
        }
        static constexpr metadatum<nation_t, int> shields_tradition()
        {
            using namespace std::literals;
            return {"shields_tradition"sv, 27, &nation_t::shields_tradition};
        }
        static constexpr metadatum<nation_t, int> stealth_tradition()
        {
            using namespace std::literals;
            return {"stealth_tradition"sv, 28, &nation_t::stealth_tradition};
        }
        static constexpr metadatum<nation_t, int> detection_tradition()
        {
            using namespace std::literals;
            return {"detection_tradition"sv, 29, &nation_t::detection_tradition};
        }
        static constexpr metadatum<nation_t, bool> defeated()
        {
            using namespace std::literals;
            return {"defeated"sv, 100, &nation_t::defeated};
        }

        template<typename F>
        static void foreach_member(F && f)
        {
            f(id());
            f(home_planet());
            f(capitol_settlement());
            f(money());
            f(unit_designs());
            f(provinces());
            f(settlements());
            f(fleets());
            f(transports());
            f(hexes_seen());
            f(systems_present_in());
            f(systems_visited());
            f(planets_surveyed());
            f(settlements_seen());
            f(foreign_designs_seen());
            f(foreign_designs_glimpsed());
            f(construction_tech());
            f(propulsion_tech());
            f(weapons_tech());
            f(shields_tech());
            f(stealth_tech());
            f(detection_tech());
            f(automation_tech());
            f(construction_tradition());
            f(propulsion_tradition());
            f(weapons_tradition());
            f(shields_tradition());
            f(stealth_tradition());
            f(detection_tradition());
            f(defeated());
        }
    };

    template<> struct metadata<date_t>
    {
        static constexpr std::string_view struct_name()
        {
            using namespace std::literals;
            return "date_t"sv;
        }
        static constexpr int lo_field_number() { return 1; }
        static constexpr int hi_field_number() { return 3; }

        static constexpr metadatum<date_t, int> year()
        {
            using namespace std::literals;
            return {"year"sv, 1, &date_t::year};
        }
        static constexpr metadatum<date_t, int> month()
        {
            using namespace std::literals;
            return {"month"sv, 2, &date_t::month};
        }
        static constexpr metadatum<date_t, int> day()
        {
            using namespace std::literals;
            return {"day"sv, 3, &date_t::day};
        }

        template<typename F>
        static void foreach_member(F && f)
        {
            f(year());
            f(month());
            f(day());
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
        static constexpr int hi_field_number() { return 9; }

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
        static constexpr metadatum<game_state_t, date_t> date()
        {
            using namespace std::literals;
            return {"date"sv, 9, &game_state_t::date};
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
            f(date());
        }
    };

    template<> struct metadata<cost_t>
    {
        static constexpr std::string_view struct_name()
        {
            using namespace std::literals;
            return "cost_t"sv;
        }
        static constexpr int lo_field_number() { return 1; }
        static constexpr int hi_field_number() { return 7; }

        static constexpr metadatum<cost_t, float> money_cost()
        {
            using namespace std::literals;
            return {"money_cost"sv, 1, &cost_t::money_cost};
        }
        static constexpr metadatum<cost_t, float> metal_cost()
        {
            using namespace std::literals;
            return {"metal_cost"sv, 2, &cost_t::metal_cost};
        }
        static constexpr metadatum<cost_t, float> energy_cost()
        {
            using namespace std::literals;
            return {"energy_cost"sv, 3, &cost_t::energy_cost};
        }
        static constexpr metadatum<cost_t, float> fuel_minerals_cost()
        {
            using namespace std::literals;
            return {"fuel_minerals_cost"sv, 4, &cost_t::fuel_minerals_cost};
        }
        static constexpr metadatum<cost_t, float> fuel_cost()
        {
            using namespace std::literals;
            return {"fuel_cost"sv, 5, &cost_t::fuel_cost};
        }
        static constexpr metadatum<cost_t, float> water_cost()
        {
            using namespace std::literals;
            return {"water_cost"sv, 6, &cost_t::water_cost};
        }
        static constexpr metadatum<cost_t, float> food_cost()
        {
            using namespace std::literals;
            return {"food_cost"sv, 7, &cost_t::food_cost};
        }

        template<typename F>
        static void foreach_member(F && f)
        {
            f(money_cost());
            f(metal_cost());
            f(energy_cost());
            f(fuel_minerals_cost());
            f(fuel_cost());
            f(water_cost());
            f(food_cost());
        }
    };

    template<> struct metadata<day_update_t>
    {
        static constexpr std::string_view struct_name()
        {
            using namespace std::literals;
            return "day_update_t"sv;
        }
        static constexpr int lo_field_number() { return 1; }
        static constexpr int hi_field_number() { return 1; }

        static constexpr metadatum<day_update_t, date_t> date()
        {
            using namespace std::literals;
            return {"date"sv, 1, &day_update_t::date};
        }

        template<typename F>
        static void foreach_member(F && f)
        {
            f(date());
        }
    };

    template<> struct metadata<month_update_t>
    {
        static constexpr std::string_view struct_name()
        {
            using namespace std::literals;
            return "month_update_t"sv;
        }
        static constexpr int lo_field_number() { return 1; }
        static constexpr int hi_field_number() { return 1; }

        static constexpr metadatum<month_update_t, date_t> date()
        {
            using namespace std::literals;
            return {"date"sv, 1, &month_update_t::date};
        }

        template<typename F>
        static void foreach_member(F && f)
        {
            f(date());
        }
    };

    template<> struct metadata<year_update_t>
    {
        static constexpr std::string_view struct_name()
        {
            using namespace std::literals;
            return "year_update_t"sv;
        }
        static constexpr int lo_field_number() { return 1; }
        static constexpr int hi_field_number() { return 1; }

        static constexpr metadatum<year_update_t, date_t> date()
        {
            using namespace std::literals;
            return {"date"sv, 1, &year_update_t::date};
        }

        template<typename F>
        static void foreach_member(F && f)
        {
            f(date());
        }
    };

}


