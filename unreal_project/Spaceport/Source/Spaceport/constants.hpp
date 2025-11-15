#pragma once

#include <cmath>
#include <numbers>


// symbolic values
// provinces
inline constexpr int prov_none = -1;
inline constexpr int prov_off_map = -2;
inline constexpr int prov_galactic_bulge = -3;
inline constexpr int prov_galactic_center = -4;
// atmospheric pressure
inline constexpr double atmos_thousands = -1.0;
inline constexpr double atmos_millions = -2.0;
// growth factor
inline constexpr double growth_uninhabitable = -1000.0;
// generic
inline constexpr int nation_none = -1;
inline constexpr double n_a = -3.0; // UI should show "N/A" for this property

// game constants
inline constexpr double base_pop_growth_factor = 0.25;
inline constexpr double prob_rocky_planet_has_magnetosphere = 0.5;
inline constexpr double prob_no_magnetosphere_rocky_planet_is_reduced = 0.5;
inline constexpr double only_equatorial_region_habitable_factor = 0.5;
inline constexpr double habs_and_masks_habitable_factor = 0.25;
inline constexpr double habs_and_masks_infra_cost_factor = 2.0;
inline constexpr double habs_and_suits_habitable_factor = 0.1;
inline constexpr double habs_and_suits_infra_cost_factor = 4.0;
inline constexpr double habs_and_suits_growth_modifier = -0.2;
inline constexpr double growth_factor_considered_habitable = 0.1;
inline constexpr double agri_equip_infra_cost_factor = 2.0;
inline constexpr int max_earth_pops = 5000;
inline constexpr int min_resource_value = 0;
inline constexpr int max_resource_value = 100;
inline constexpr int energy_from_solar = 25;
inline constexpr int energy_from_wind = 25;
inline constexpr double gas_giant_moon_resource_factor = 0.1;
inline constexpr int default_systems_per_hex = 100;

// math constants
inline const double sin_60 = std::sin(std::numbers::pi / 3.0);

// These assume a hex's corners are 1.0 from the center.
inline constexpr double hex_width = 1.5;
inline const double hex_height = 2 * sin_60;

// (real-world) physical constants
constexpr double G_ = 6.67e-11; // gravitaional constant, in N * m^2 / kg^2
constexpr double sun_temperature_k = 5772.0;
constexpr double sun_radius_km = 696342.0;
constexpr double sun_mass_kg = 1.988416e30;
constexpr double m_per_km = 1000.0;
constexpr double km_per_au = 149597870.7;
constexpr double secs_per_year = 31556952.0;
constexpr double earth_temperature_k = 279; // in K, ignoring albedo
constexpr double earth_mass_kg = 5.9722e24;
constexpr double earth_gravity = 9.8; // in m / s^2
constexpr double earth_radius_km = 6371.0;
constexpr double earth_ocean_coverage = 0.71;
constexpr double earth_o2_percentage = 20.9;
constexpr double harmless_low_o2_percentage = 19.5;
// See: https://hypoxico.com/pages/altitude-to-oxygen-chart
constexpr double effective_o2_percentage_la_paz_bolivia = 13.2;
constexpr double effective_o2_percentage_aconcagua = 8.7;
constexpr double effective_o2_percentage_mt_everest_peak = 6.9;
constexpr double mass_of_solar_system_planets_kg =
    3.3e23 + 4.87e24 + 5.97e24 + 6.42e23 +
    1.90e27 + 5.68e26 + 1.02e26 + 8.68e25;
