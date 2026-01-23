#include "constants.hpp"

#include "lua.hpp"

#include <cmath>
#include <numbers>


const double G_ = 6.67e-11;
const double sun_temperature_k = 5772.0;
const double sun_radius_km = 696342.0;
const double sun_mass_kg = 1.988416e30;
const double m_per_km = 1000.0;
const double km_per_au = 149597870.7;
const double secs_per_year = 31556952.0;
const double earth_temperature_k = 279;
const double earth_mass_kg = 5.9722e24;
const double earth_gravity = 9.8;
const double earth_radius_km = 6371.0;
const double earth_ocean_coverage = 0.71;
const double earth_o2_percentage = 20.9;
const double harmless_low_o2_percentage = 19.5;
const double effective_o2_percentage_la_paz_bolivia = 13.2;
const double effective_o2_percentage_aconcagua = 8.7;
const double effective_o2_percentage_mt_everest_peak = 6.9;
const double mass_of_solar_system_planets_kg = 3.3e23 + 4.87e24 + 5.97e24 +
                                               6.42e23 + 1.90e27 + 5.68e26 +
                                               1.02e26 + 8.68e25;

const double sin_60 = std::sin(std::numbers::pi / 3.0);
const double epsilon = 1.0e-3;

const double hex_width = 1.5;
const double hex_height = 2 * sin_60;

const int prov_none = make_prov_none();
const int prov_off_map = make_prov_off_map();
const int prov_galactic_bulge = make_prov_galactic_bulge();
const int prov_galactic_center = make_prov_galactic_center();
const double atmos_thousands = make_atmos_thousands();
const double atmos_millions = make_atmos_millions();
const double growth_uninhabitable = make_growth_uninhabitable();
const int nation_none = make_nation_none();
const int hex_none = make_hex_none();
const int system_none = make_system_none();
const int object_none = make_object_none();
const double n_a = make_n_a();

const float base_pop_growth_factor = lua()["base_pop_growth_factor"];
const double prob_rocky_planet_has_magnetosphere =
    lua()["prob_rocky_planet_has_magnetosphere"];
const double prob_no_magnetosphere_rocky_planet_is_reduced =
    lua()["prob_no_magnetosphere_rocky_planet_is_reduced"];
const double ice_giant_distance_factor = lua()["ice_giant_distance_factor"];
const double day_length_factor = lua()["day_length_factor"];
const float only_equatorial_region_habitable_factor =
    lua()["only_equatorial_region_habitable_factor"];
const float habs_and_masks_habitable_factor =
    lua()["habs_and_masks_habitable_factor"];
const float habs_and_masks_infra_cost_factor =
    lua()["habs_and_masks_infra_cost_factor"];
const float habs_and_suits_habitable_factor =
    lua()["habs_and_suits_habitable_factor"];
const float habs_and_suits_infra_cost_factor =
    lua()["habs_and_suits_infra_cost_factor"];
const float habs_and_suits_growth_modifier =
    lua()["habs_and_suits_growth_modifier"];
const float growth_factor_considered_habitable =
    lua()["growth_factor_considered_habitable"];
const float agri_equip_infra_cost_factor =
    lua()["agri_equip_infra_cost_factor"];
const int max_earth_pops = lua()["max_earth_pops"];
const int min_resource_value = lua()["min_resource_value"];
const int max_resource_value = lua()["max_resource_value"];
const int energy_from_solar = lua()["energy_from_solar"];
const int energy_from_wind = lua()["energy_from_wind"];
const double gas_giant_moon_resource_factor =
    lua()["gas_giant_moon_resource_factor"];
const int default_systems_per_hex = lua()["default_systems_per_hex"];
const int max_infrastructure = lua()["max_infrastructure"];
const double high_temp_k = lua()["high_temp_k"];
const double high_temp_atmosphere_pressure_factor =
    lua()["high_temp_atmosphere_pressure_factor"];
const double reduced_rocky_planet_pressure_factor =
    lua()["reduced_rocky_planet_pressure_factor"];
const double nonreduced_rocky_planet_pressure_factor =
    lua()["nonreduced_rocky_planet_pressure_factor"];
const double min_habitable_nonsuit_temp_k =
    lua()["min_habitable_nonsuit_temp_k"];
const double max_habitable_temp_k = lua()["max_habitable_temp_k"];
const double mean_starting_pop = lua()["mean_starting_pop"];
const double starting_pop_plus_minus = lua()["starting_pop_plus_minus"];

const int small_map_height = lua()["small_map_height"];
const int medium_map_height = lua()["medium_map_height"];
const int large_map_height = lua()["large_map_height"];
const int starting_tech_level = lua()["starting_tech_level"];
const int starting_engineering_tradition =
    lua()["starting_engineering_tradition"];

const float map_actors_vertical_offset = -25000.0f;
const float just_inside_system_map = map_actors_vertical_offset + 50;
const float min_camera_dist =
    float(lua()["min_camera_dist"]) - map_actors_vertical_offset;
const float max_camera_dist =
    float(lua()["max_camera_dist"]) - map_actors_vertical_offset;
const float camera_arm_move_speed =
    std::max(1.0f, float(lua()["camera_arm_move_speed"]));
const float max_double_click_interval_s = lua()["max_double_click_interval_s"];
const float ui_fade_in_time_s = lua()["ui_fade_in_time_s"];
const float system_view_transition_time_s =
    std::max(float(lua()["system_view_transition_time_s"]), ui_fade_in_time_s);
const float map_move_modififier_factor = lua()["map_move_modififier_factor"];
const float system_map_move_factor = lua()["system_map_move_factor"];
const float system_map_zoom_to_time_s = lua()["system_map_zoom_to_time_s"];
const float min_time_between_day_ticks_s =
    lua()["min_time_between_day_ticks_s"];

// unit design
const float max_armor_per_hull_point = lua()["max_armor_per_hull_point"];
const float negligible_hull_size_design_difference =
    lua()["negligible_hull_size_design_difference"];
const float close_to_max_hull_size = lua()["close_to_max_hull_size"];
const float base_unit_design_months = lua()["base_unit_design_months"];
const float unit_design_months_per_novelty_point =
    lua()["unit_design_months_per_novelty_point"];
const float base_unit_design_monthly_cost_factor =
    lua()["base_unit_design_monthly_cost_factor"];
const float unit_design_monthly_cost_factor_per_novelty_point =
    lua()["unit_design_monthly_cost_factor_per_novelty_point"];
const double max_unit_design_space_factor =
    lua()["max_unit_design_space_factor"];
const double min_unit_design_effectiveness_factor =
    lua()["min_unit_design_effectiveness_factor"];
const int space_required_per_equipment_point =
    lua().script("return to_integer(space_required_per_equipment_point)");
const int pd_volleys_per_unit_storage =
    lua().script("return to_integer(pd_volleys_per_unit_storage)");
const int missile_volleys_per_unit_storage =
    lua().script("return to_integer(missile_volleys_per_unit_storage)");

// combat
const int fuel_explosion_radius =
    lua().script("return to_integer(fuel_explosion_radius)");
const int ammo_explosion_radius =
    lua().script("return to_integer(ammo_explosion_radius)");
const int base_pd_attack_factor =
    lua().script("return to_integer(base_pd_attack_factor)");
const int pd_hit_weapons_factor =
    lua().script("return to_integer(pd_hit_weapons_factor)");
const int pd_hit_acceleration_factor =
    lua().script("return to_integer(pd_hit_acceleration_factor)");
const int base_missile_attack_factor =
    lua().script("return to_integer(base_missile_attack_factor)");
const int missile_hit_weapons_factor =
    lua().script("return to_integer(missile_hit_weapons_factor)");
const int missile_hit_acceleration_factor =
    lua().script("return to_integer(missile_hit_acceleration_factor)");
const int screen_combat_selection_factor =
    lua().script("return to_integer(screen_combat_selection_factor)");
const int capital_combat_selection_factor =
    lua().script("return to_integer(capital_combat_selection_factor)");
const double keep_previous_combat_target_probability =
    lua()["keep_previous_combat_target_probability"];
const float crew_required_per_hull_point =
    lua()["crew_required_per_hull_point"];
const float crew_required_per_equipment_point =
    lua()["crew_required_per_equipment_point"];
const float space_required_per_1k_crew = lua()["space_required_per_1k_crew"];
const float crew_onduty_factor = 1 / 3.0f;
const float minimum_viable_crew_factor = 1 / 3.0f;
const float unit_combat_experience_impact =
    lua()["unit_combat_experience_impact"];
const float fleet_combat_experience_impact =
    lua()["fleet_combat_experience_impact"];

#if !defined(BUILD_FOR_TEST)
const ECollisionChannel fleet_channel =
    ECollisionChannel::ECC_GameTraceChannel1;
const ECollisionChannel star_channel = ECollisionChannel::ECC_GameTraceChannel2;
const ECollisionChannel hex_channel = ECollisionChannel::ECC_GameTraceChannel3;
#endif

int make_prov_none() { return -1; }
int make_prov_off_map() { return -2; }
int make_prov_galactic_bulge() { return -3; }
int make_prov_galactic_center() { return -4; }
double make_atmos_thousands() { return -1234.0; }
double make_atmos_millions() { return -2345.0; }
double make_growth_uninhabitable() { return -1000.0; }
int make_nation_none() { return -1; }
int make_hex_none() { return -1; }
int make_system_none() { return -1; }
int make_object_none() { return -1; }
double make_n_a()
{
    unsigned int mantissa = 1u << FLT_MANT_DIG;
    return -double(mantissa);
}
