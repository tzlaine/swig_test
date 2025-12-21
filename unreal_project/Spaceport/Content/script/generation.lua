function apply_planet_effect(planet, e)
   effect = planet_effects[e.name:c_str()]
   instrument(level.trace, e)
   instrument(level.trace, effect)
   if effect.value then
      effect:apply(planet)
   else
      effect:apply(planet, e.value)
   end
end

function determine_growth_factor_and_effects(planet)
   local effect = function(planet, name)
      pe = planet_effect_t.new(name)
      planet.effects:add(pe)
   end

   instrument(level.trace, 'cp 0')

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

   instrument(level.trace, 'cp 1')

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

   instrument(level.trace, 'cp 2')

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

   instrument(level.trace, 'cp 3')

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

   instrument(level.trace, 'cp 4')

   -- O2 (making simplifying assumption of ignoring the CO2 part of this
   -- property)
   local harmless_o2_threshold = harmless_low_o2_percentage / earth_o2_percentage
   local effective_o2 = planet.o2_co2_suitability * planet.atmospheric_pressure
   if harmless_o2_threshold < effective_o2 then
      -- no effect
   elseif effective_o2_percentage_la_paz_bolivia / earth_o2_percentage < effective_o2 then
      instrument(level.trace, 'cp 4.1')
      effect(planet, 'poor_o2_co2_suitab')
   elseif effective_o2_percentage_aconcagua / earth_o2_percentage < effective_o2 then
      instrument(level.trace, 'cp 4.2.0')
      effect(planet, 'very_poor_o2_co2_suitab')
      instrument(level.trace, 'cp 4.2.1')
      habs_and_masks_required(planet, 'very_poor_o2_co2_suitab')
   elseif effective_o2_percentage_mt_everest_peak / earth_o2_percentage < effective_o2 then
      instrument(level.trace, 'cp 4.3')
      effect(planet, 'marginal_o2_co2_suitab')
      habs_and_masks_required(planet, 'marginal_o2_co2_suitab')
   else
      instrument(level.trace, 'cp 4.4')
      habs_and_suits_required(planet, 'insufficient_o2_co2_suitab')
      effect(planet, 'insufficient_o2_co2_suitab')
   end

   instrument(level.trace, 'cp 5')

   -- atmospheric pressure (< 1 cases handled with o2_co2_suitability above)
   if 4.0 < planet.atmospheric_pressure then
      habs_and_suits_required(planet, 'high_press_n2_narcosis')
      effect(planet, 'high_press_n2_narcosis')
   end
   if 7.0 < effective_o2 then
      habs_and_suits_required(planet, 'very_high_press_o2_toxicity')
      effect(planet, 'very_high_press_o2_toxicity')
   end

   instrument(level.trace, 'cp 6')

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

   instrument(level.trace, 'cp 7')

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

   instrument(level.trace, 'cp 8')

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

   instrument(level.trace, 'cp 9')

   for i = 1, #planet.effects do
      apply_planet_effect(planet, planet.effects:get(i))
   end

   instrument(level.trace, 'cp 10')

   return planet.growth_factor
end
