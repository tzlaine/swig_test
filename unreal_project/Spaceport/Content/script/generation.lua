function apply_planet_effect(planet, e)
   local effect = planet_effects[e.name:c_str()]
   if effect.apply then
      if effect.value then
         effect:apply(planet)
      else
         effect:apply(planet, e.value)
      end
      return
   end
   local apply = nil
   if effect.target == 'growth_factor' then
      apply = function(planet, value)
         local x = effect_value(planet, value)
         planet.growth_factor = planet.growth_factor + x
      end
   elseif effect.target == 'max_population' then
      apply = function(planet, value)
         local x = effect_value(planet, value)
         planet.max_population = to_integer(planet.max_population * x)
      end
   elseif effect.target == 'infrastructure_cost' then
      apply = function(planet, value)
         local x = effect_value(planet, value)
         planet.infrastructure_cost_factor =
            planet.infrastructure_cost_factor * x
      end
   end
   apply(planet, effect.value and effect.value or e.value)
end

function determine_growth_factor_and_effects(planet)
   local effect = function(planet, name)
      local pe = planet_effect_t.new(name)
      planet.effects:add(pe)
   end

   planet.growth_factor = base_pop_growth_factor

   if planet.planet_type ~= planet_type_t.rocky then
      effect(planet, 'uninhab_non_rocky_planet')
      planet.growth_factor = planet.growth_factor + growth_uninhabitable
      return planet.growth_factor
   end

   -- MAINTENANCE NOTE: Any calls to the habs_and_suits_required (but not the
   -- masks one) should come before any other effects, since it clears out all
   -- previous effects.  Any effects that affect colonists living in
   -- habs+suits should come *after* all calls to habs_and_suits_required.

   -- tilt
   if planet.axial_tilt_d < 5.0 then
      effect(planet, 'no_seasons')
   elseif planet.axial_tilt_d < 15.0 then
      effect(planet, 'mild_seasons')
   elseif planet.axial_tilt_d < 30.0 then
      -- normal seasons
      if planet.orbital_period_y < 0.8 then
         effect(planet, 'short_seasons')
      elseif 1.2 < planet.orbital_period_y then
         effect(planet, 'long_seasons')
         planet.effects:add(
            planet_effect_t.new(
               'infra_cost',
               'long_seasons',
               agri_equip_infra_cost_factor))
         if 3.0 < planet.orbital_period_y then
            effect(planet, 'only_equatorial_band_habitable')
         end
      end
   elseif planet.axial_tilt_d < 45.0 then
      effect(planet, 'intense_seasons')
      if planet.orbital_period_y < 0.8 then
         effect(planet, 'short_seasons')
      elseif 1.2 < planet.orbital_period_y then
         effect(planet, 'long_seasons')
         planet.effects:add(
            planet_effect_t.new(
               'infra_cost',
               'long_intense_seasons',
               1.5))
         if 1.5 < planet.orbital_period_y then
            effect(planet, 'only_equatorial_band_habitable')
         end
      end
   elseif planet.axial_tilt_d < 60.0 then
      effect(planet, 'intense_seasons')
      if planet.orbital_period_y < 0.8 then
         effect(planet, 'short_seasons')
      elseif 1.2 < planet.orbital_period_y then
         effect(planet, 'long_seasons')
         planet.effects:add(
            planet_effect_t.new(
               'infra_cost',
               'long_intense_seasons',
               1.5))
      end
      effect(planet, 'only_equatorial_band_habitable')
   elseif planet.axial_tilt_d < 75.0 then
      if planet.orbital_period_y < 0.8 then
         effect(planet, 'short_seasons')
      elseif 1.2 < planet.orbital_period_y then
         effect(planet, 'long_seasons')
         planet.effects:add(
            planet_effect_t.new(
               'infra_cost',
               'long_seasons',
               agri_equip_infra_cost_factor))
      end
      effect(planet, 'only_equatorial_band_habitable')
   elseif planet.axial_tilt_d < 85.0 then
      effect(planet, 'mild_seasons')
      effect(planet, 'only_equatorial_band_habitable')
   else
      effect(planet, 'no_seasons')
      effect(planet, 'only_equatorial_band_habitable')
   end

   -- day length
   if planet.day_h < 24.0 * 0.9 then
      effect(planet, 'short_days')
      planet.effects:add(
         planet_effect_t.new(
            'infra_cost',
            'short_days',
            agri_equip_infra_cost_factor))
   elseif planet.day_h < 24.0 * 1.1 then
      -- no effect
   elseif planet.day_h < 48.0 then
      effect(planet, 'long_days')
      planet.effects:add(
         planet_effect_t.new(
            'infra_cost',
            'long_days',
            agri_equip_infra_cost_factor))
   else
      effect(planet, 'very_long_days')
      planet.effects:add(
         planet_effect_t.new(
            'infra_cost',
            'very_long_days',
            agri_equip_infra_cost_factor))
   end

   -- TODO: Require habs+suits if the day is long enough?

   -- TODO: Should the day length effects apply to planets that are very
   -- highly tilted, like >= 75 deg?

   local habs_and_suits_already_required = 0
   local habs_and_masks_required = function(planet, reason)
      planet.effects:add(
         planet_effect_t.new('habs_and_masks_required', reason))
      planet.effects:add(
         planet_effect_t.new(
            'infra_cost',
            reason,
            habs_and_masks_infra_cost_factor))
   end
   local habs_and_suits_required = function(planet, reason)
      if 0 < habs_and_suits_already_required then
         return
      end

      -- Dump all previous effects.  Living in a hab+suit obviates almost all
      -- other effects.
      planet.effects:clear()

      planet.effects:add(
         planet_effect_t.new('habs_and_suits_required', reason))
      planet.effects:add(
         planet_effect_t.new(
            'infra_cost',
            reason,
            habs_and_suits_infra_cost_factor))

      habs_and_suits_already_required = habs_and_suits_already_required + 1
   end

   -- O2 (making simplifying assumption of ignoring the CO2 part of this
   -- property)
   local harmless_o2_threshold = harmless_low_o2_percentage / earth_o2_percentage
   local effective_o2 = planet.o2_co2_suitability * planet.atmospheric_pressure
   if harmless_o2_threshold < effective_o2 then
      -- no effect
   elseif effective_o2_percentage_la_paz_bolivia / earth_o2_percentage < effective_o2 then
      effect(planet, 'poor_o2_co2_suitab')
   elseif effective_o2_percentage_aconcagua / earth_o2_percentage < effective_o2 then
      effect(planet, 'very_poor_o2_co2_suitab')
      habs_and_masks_required(planet, 'very_poor_o2_co2_suitab')
   elseif effective_o2_percentage_mt_everest_peak / earth_o2_percentage < effective_o2 then
      effect(planet, 'marginal_o2_co2_suitab')
      habs_and_masks_required(planet, 'marginal_o2_co2_suitab')
   else
      habs_and_suits_required(planet, 'insufficient_o2_co2_suitab')
      effect(planet, 'insufficient_o2_co2_suitab')
   end

   -- atmospheric pressure (< 1 cases handled with o2_co2_suitability above)
   if 4.0 < planet.atmospheric_pressure then
      habs_and_suits_required(planet, 'high_press_n2_narcosis')
      effect(planet, 'high_press_n2_narcosis')
   end
   if 7.0 < planet.atmospheric_pressure and harmless_o2_threshold < effective_o2 then
      habs_and_suits_required(planet, 'very_high_press_o2_toxicity')
      effect(planet, 'very_high_press_o2_toxicity')
   end

   -- magnetosphere
   if planet.magnetosphere_strength < 0.33 then
      habs_and_suits_required(planet, 'very_weak_magneto')
      effect(planet, 'very_weak_magneto')
   elseif planet.magnetosphere_strength < 0.9 then
      effect(planet, 'weak_magneto')
      if planet.magnetosphere_strength < 0.67 then
         habs_and_masks_required(planet, 'weak_magneto')
      end
   elseif planet.magnetosphere_strength < 1.1 then
      -- no effect
   elseif 1.1 < planet.magnetosphere_strength then
      effect(planet, 'strong_magneto')
   end

   -- temperature
   if planet.surface_temperature_k < min_habitable_nonsuit_temp_k then
      habs_and_suits_required(planet, 'extremely_cold_avg_surface_temp')
      effect(planet, 'extremely_cold_avg_surface_temp')
   elseif planet.surface_temperature_k < earth_temperature_k - 22 then
      effect(planet, 'very_cold_avg_surface_temp')
   elseif planet.surface_temperature_k < earth_temperature_k - 11 then
      effect(planet, 'cold_avg_surface_temp')
   elseif planet.surface_temperature_k < earth_temperature_k + 11 then
      -- no effect
   elseif planet.surface_temperature_k < earth_temperature_k + 22 then
      effect(planet, 'hot_avg_surface_temp')
   elseif planet.surface_temperature_k < earth_temperature_k + 33 then
      effect(planet, 'very_hot_avg_surface_temp')
   elseif planet.surface_temperature_k < max_habitable_temp_k then
      habs_and_suits_required(planet, 'extremely_hot_avg_surface_temp')
      effect(planet, 'extremely_hot_avg_surface_temp')
   else
      effect(planet, 'uninhabitably_hot_avg_surface_temp')
   end

   -- gravity
   if planet.gravity_g < 0.1 then
      effect(planet, 'very_low_grav')
   elseif planet.gravity_g < 0.9 then
      effect(planet, 'low_grav')
   elseif planet.gravity_g < 1.1 then
      -- no effect
   elseif planet.gravity_g < 1.3 then
      effect(planet, 'high_grav')
   else
      effect(planet, 'very_high_grav')
   end

   for i = 1, #planet.effects do
      apply_planet_effect(planet, planet.effects[i])
   end

   return planet.growth_factor
end

function starting_planet_score(planet)
   if planet.growth_factor < growth_factor_considered_habitable or
      planet.metal < 0.2 * max_resource_value or
      planet.fuel_minerals < 0.2 * max_resource_value or
      mean_starting_pop + starting_pop_plus_minus < planet.max_population then
      return -1000.0
   end

   return planet.max_population
end

function starting_small_ship_design(nation)
   local retval = unit_design_t.new()
   retval.id = nation_and_object_id_t.new(nation.id, 0)
   retval.hull = 10
   retval.armor = 0
   retval.propulsion = 1
   retval.weapons = 1
   retval.shields = 1
   retval.detection = 1
   retval.stealth = 1
   retval.automation = 1
   retval.attack = 2
   retval.defense = 2
   retval.ground_attack = 0
   return retval
end
function starting_large_ship_design(nation)
   local retval = unit_design_t.new()
   retval.id = nation_and_object_id_t.new(nation.id, 1)
   retval.hull = 25
   retval.armor = 1
   retval.propulsion = 1
   retval.weapons = 1
   retval.shields = 1
   retval.detection = 1
   retval.stealth = 1
   retval.automation = 1
   retval.attack = 4
   retval.defense = 4
   retval.ground_attack = 0
   return retval
end
function starting_colony_ship_design(nation)
   local retval = unit_design_t.new()
   retval.id = nation_and_object_id_t.new(nation.id, 2)
   retval.hull = 50
   retval.armor = 0
   retval.propulsion = 1
   retval.weapons = 1
   retval.shields = 1
   retval.detection = 1
   retval.stealth = 1
   retval.automation = 1
   retval.attack = 0
   retval.defense = 0
   retval.ground_attack = 0
   return retval
end
function starting_lifter_design(nation)
   local retval = unit_design_t.new()
   retval.id = nation_and_object_id_t.new(nation.id, 3)
   retval.hull = 40
   retval.armor = 0
   retval.propulsion = 1
   retval.weapons = 1
   retval.shields = 1
   retval.detection = 1
   retval.stealth = 1
   retval.automation = 1
   retval.attack = 0
   retval.defense = 0
   retval.ground_attack = 0
   return retval
end

function assign_starting_designs(nation)
   nation.unit_designs:add(starting_small_ship_design(nation))
   nation.unit_designs:add(starting_large_ship_design(nation))
   nation.unit_designs:add(starting_colony_ship_design(nation))
   nation.unit_designs:add(starting_lifter_design(nation))
end

function create_home_settlement(gs, nation, planet, planet_id)
   local retval = settlement_t.new()
   retval.id = nation_and_object_id_t.new(nation.id, 0)
   retval.planet_id = planet_id
   retval.original_owner = nation.id
   retval.population = planet.max_population
   retval.infrastructure = 100
   retval.water = resource_t.new()
   retval.water.availability = planet.water
   retval.water.max_availability = planet.water
   retval.food = resource_t.new()
   retval.food.availability = planet.food
   retval.food.max_availability = planet.food
   retval.energy = resource_t.new()
   retval.energy.availability = planet.energy
   retval.energy.max_availability = planet.energy
   retval.metal = resource_t.new()
   retval.metal.availability = planet.metal
   retval.metal.max_availability = planet.metal
   retval.fuel_minerals = resource_t.new()
   retval.fuel_minerals.availability = planet.fuel_minerals
   retval.fuel_minerals.max_availability = planet.fuel_minerals
   return retval
end

function create_starting_fleet(gs, nation, planet)
   local retval = fleet_t.new()
   retval.id = nation_and_object_id_t.new(nation.id, 0)
   retval.mission = mission_t.no_mission

   local unit = unit_t.new()
   unit.id = starting_small_ship_design(nation).id
   unit.health = 100
   retval.units:add(unit)
   retval.units:add(unit)
   retval.units:add(unit)

   unit = unit_t.new()
   unit.id = starting_large_ship_design(nation).id
   unit.health = 100
   retval.units:add(unit)

   unit = unit_t.new()
   unit.id = starting_colony_ship_design(nation).id
   unit.health = 100
   retval.units:add(unit)

   retval.fuel = 100
   retval.rounds = 100

   retval.position = fleet_position_t.new()
   local system = planet_system(gs, planet)
   retval.position.world_pos_x = system.world_pos_x
   retval.position.world_pos_y = system.world_pos_y
   retval.position.system_id = planet.system_id
   retval.position.at_permanent_location = true
   retval.position.location_index = 0
   retval.position.object_index = 0

   return retval
end

-- TODO: These need to move to another file for reuse.
function planet_system(gs, planet)
   return gs.systems[planet.system_id + 1]
end
function system_hex(gs, system)
   return gs.hexes[system.hex_id + 1]
end
function hc_on_map(gs, hc)
   return cpp_on_map(hc, to_integer(gs.map_width), to_integer(gs.map_height))
end
function hc_to_index(gs, hc)
   return cpp_to_index(hc, gs.map_width)
end
function hc_from_index(gs, i)
   return cpp_from_index(i, gs.map_width)
end
function foreach_onmap_hc_around(gs, center_hc, f)
   local hc = nil
   hc = hex_above_right(center_hc)
   if hc_on_map(gs, hc) then
      f(hc)
   end
   hc = hex_above(center_hc)
   if hc_on_map(gs, hc) then
      f(hc)
   end
   hc = hex_above_left(center_hc)
   if hc_on_map(gs, hc) then
      f(hc)
   end
   hc = hex_below_left(center_hc)
   if hc_on_map(gs, hc) then
      f(hc)
   end
   hc = hex_below(center_hc)
   if hc_on_map(gs, hc) then
      f(hc)
   end
   hc = hex_below_right(center_hc)
   if hc_on_map(gs, hc) then
      f(hc)
   end
end

function create_starting_nation(gs, nation_id, home_planet, planet_id)
   nation = nation_t.new()
   nation.id = nation_id
   nation.home_planet = planet_id
   nation.capitol_settlement = nation_and_object_id_t.new(nation_id, 0)
   assign_starting_designs(nation)
   nation.settlements:add(create_home_settlement(gs, nation, home_planet, planet_id))
   nation.fleets:add(create_starting_fleet(gs, nation, home_planet))
   -- TODO: Add a fleet representing the starting outpost.
   -- TODO: Add starting garrison(s)?

   local home_hc = system_hex(gs, planet_system(gs, home_planet)).coord
   nation.hexes_seen:add(hc_to_index(gs, home_hc))
   foreach_onmap_hc_around(
      gs, home_hc, function(hc)
         nation.hexes_seen:add(hc_to_index(gs, hc))
      end
   )

   nation.systems_present_in:add(home_planet.system_id)
   nation.systems_visited:add(home_planet.system_id)
   local system = planet_system(gs, home_planet)
   local system_name_index = random_int(1, #system_names)
   while system_names[system_name_index] == '' do
      system_name_index = random_int(1, #system_names)
   end
   system.name = name_t.new(system_names[system_name_index])
   system_names[system_name_index] = ''
   for i = system.first_planet, system.last_planet - 1 do
      nation.planets_surveyed:add(i)
   end

   local location = system_location_t.new()
   local location_object = location_object_t.new()
   location_object.planet_id = planet_id
   -- TODO: Add starting outpost here.
   location.objects:add(location_object)
   system.permanent_locations:add(location)

   gs.nations:add(nation)
end
