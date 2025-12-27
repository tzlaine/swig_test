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

const float min_camera_dist = lua()["min_camera_dist"];
const float max_camera_dist = lua()["max_camera_dist"];
const float max_double_click_interval_s = lua()["max_double_click_interval_s"];

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
double make_n_a() { return -4321.0; }
