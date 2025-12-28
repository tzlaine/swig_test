-- **************
-- game constants
-- **************

-- galaxy generation
base_pop_growth_factor = 0.25
prob_rocky_planet_has_magnetosphere = 0.5
prob_no_magnetosphere_rocky_planet_is_reduced = 0.5
ice_giant_distance_factor = 15.0 -- ~AUs between the Saturn and Uranus orbits
day_length_factor = 15.0
only_equatorial_region_habitable_factor = 0.5
habs_and_masks_habitable_factor = 0.25
habs_and_masks_infra_cost_factor = 2.0
habs_and_suits_habitable_factor = 0.1
habs_and_suits_infra_cost_factor = 4.0
habs_and_suits_growth_modifier = -0.2
growth_factor_considered_habitable = 0.1
agri_equip_infra_cost_factor = 2.0
max_earth_pops = 5000
min_resource_value = 0
max_resource_value = 100
energy_from_solar = 25
energy_from_wind = 25
gas_giant_moon_resource_factor = 0.1
default_systems_per_hex = 20
max_infrastructure = 100
high_temp_k = earth_temperature_k + 50
high_temp_atmosphere_pressure_factor = 5.0
reduced_rocky_planet_pressure_factor = 0.05
nonreduced_rocky_planet_pressure_factor = 0.2
min_habitable_nonsuit_temp_k = earth_temperature_k - 44
max_habitable_temp_k = earth_temperature_k + 55
mean_starting_pop = max_earth_pops / 2
starting_pop_plus_minus = mean_starting_pop / 10

-- game start parameters
small_map_height = 11
medium_map_height = 21
large_map_height = 41

-- camera
min_camera_dist = 50.0
max_camera_dist = 5000.0
max_double_click_interval_s = 0.5
system_view_transition_time_s = 0.75
