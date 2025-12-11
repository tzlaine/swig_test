#pragma once

#if !defined(BUILD_FOR_TEST)
#include <Engine/EngineTypes.h>
#endif


// (real-world) physical constants
extern const double G_; // gravitaional constant, in N * m^2 / kg^2
extern const double sun_temperature_k;
extern const double sun_radius_km;
extern const double sun_mass_kg;
extern const double m_per_km;
extern const double km_per_au;
extern const double secs_per_year;
extern const double earth_temperature_k; // in K, ignoring albedo
extern const double earth_mass_kg;
extern const double earth_gravity; // in m / s^2
extern const double earth_radius_km;
extern const double earth_ocean_coverage;
extern const double earth_o2_percentage;
extern const double harmless_low_o2_percentage;
// See: https://hypoxico.com/pages/altitude-to-oxygen-chart
extern const double effective_o2_percentage_la_paz_bolivia;
extern const double effective_o2_percentage_aconcagua;
extern const double effective_o2_percentage_mt_everest_peak;
extern const double mass_of_solar_system_planets_kg;

// symbolic values
// provinces
extern const int prov_none;
extern const int prov_off_map;
extern const int prov_galactic_bulge;
extern const int prov_galactic_center;
// atmospheric pressure
extern const double atmos_thousands;
extern const double atmos_millions;
// growth factor
extern const double growth_uninhabitable;
// generic
extern const int nation_none;
extern const int system_none;
extern const double n_a; // UI should show "N/A" for this property

// game constants
extern const float base_pop_growth_factor;
extern const double prob_rocky_planet_has_magnetosphere;
extern const double prob_no_magnetosphere_rocky_planet_is_reduced;
extern const float only_equatorial_region_habitable_factor;
extern const float habs_and_masks_habitable_factor;
extern const float habs_and_masks_infra_cost_factor;
extern const float habs_and_suits_habitable_factor;
extern const float habs_and_suits_infra_cost_factor;
extern const float habs_and_suits_growth_modifier;
extern const float growth_factor_considered_habitable;
extern const float agri_equip_infra_cost_factor;
extern const int max_earth_pops;
extern const int min_resource_value;
extern const int max_resource_value;
extern const int energy_from_solar;
extern const int energy_from_wind;
extern const double gas_giant_moon_resource_factor;
extern const int default_systems_per_hex;
extern const int max_infrastructure;
extern const double min_habitable_nonsuit_temp_k;
extern const double max_habitable_temp_k;
extern const int small_map_height;
extern const int medium_map_height;
extern const int large_map_height;
extern const float min_camera_dist;
extern const float max_camera_dist;

#if !defined(BUILD_FOR_TEST)
extern const ECollisionChannel fleet_channel;
extern const ECollisionChannel star_channel;
extern const ECollisionChannel hex_channel;
#endif

// math constants
extern const double sin_60;
extern const double epsilon;

// These assume a hex's corners are 1.0 from the center.
extern const double hex_width;
extern const double hex_height;
