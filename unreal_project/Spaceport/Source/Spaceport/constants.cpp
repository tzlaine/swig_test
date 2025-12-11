#include "constants.hpp"

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
const double mass_of_solar_system_planets_kg =
    3.3e23 + 4.87e24 + 5.97e24 + 6.42e23 +
    1.90e27 + 5.68e26 + 1.02e26 + 8.68e25;

const int prov_none = -1;
const int prov_off_map = -2;
const int prov_galactic_bulge = -3;
const int prov_galactic_center = -4;
const double atmos_thousands = -1.0;
const double atmos_millions = -2.0;
const double growth_uninhabitable = -1000.0;
const int nation_none = -1;
const int hex_none = -1;
const int system_none = -1;
const int object_none = -1;
const double n_a = -3.0;

const float base_pop_growth_factor = 0.25;
const double prob_rocky_planet_has_magnetosphere = 0.5;
const double prob_no_magnetosphere_rocky_planet_is_reduced = 0.5;
const float only_equatorial_region_habitable_factor = 0.5;
const float habs_and_masks_habitable_factor = 0.25;
const float habs_and_masks_infra_cost_factor = 2.0;
const float habs_and_suits_habitable_factor = 0.1;
const float habs_and_suits_infra_cost_factor = 4.0;
const float habs_and_suits_growth_modifier = -0.2;
const float growth_factor_considered_habitable = 0.1;
const float agri_equip_infra_cost_factor = 2.0;
const int max_earth_pops = 5000;
const int min_resource_value = 0;
const int max_resource_value = 100;
const int energy_from_solar = 25;
const int energy_from_wind = 25;
const double gas_giant_moon_resource_factor = 0.1;
const int default_systems_per_hex = 20;
const int max_infrastructure = 100;
const double min_habitable_nonsuit_temp_k = earth_temperature_k - 44;
const double max_habitable_temp_k = earth_temperature_k + 55;
const int small_map_height = 11;
const int medium_map_height = 21;
const int large_map_height = 41;
const float min_camera_dist = 50.0f;
const float max_camera_dist = 5000.0f;

#if !defined(BUILD_FOR_TEST)
const ECollisionChannel fleet_channel =
    ECollisionChannel::ECC_GameTraceChannel1;
const ECollisionChannel star_channel = ECollisionChannel::ECC_GameTraceChannel2;
const ECollisionChannel hex_channel = ECollisionChannel::ECC_GameTraceChannel3;
#endif

const double sin_60 = std::sin(std::numbers::pi / 3.0);
const double epsilon = 1.0e-3;

const double hex_width = 1.5;
const double hex_height = 2 * sin_60;
