function eval_effect_value(obj, value)
   if type(value) == 'function' then
      return value(obj)
   else
      return value
   end
end

function growth_factor_add(planet, value)
   x = eval_effect_value(planet, value)
   planet.growth_factor = planet.growth_factor + x
end
function max_pop_mul(planet, value)
   x = eval_effect_value(planet, value)
   planet.max_population = math.tointeger(planet.max_population * x)
end
function infra_mul(planet, value)
   x = eval_effect_value(planet, value)
   planet.infrastructure_cost_factor = planet.infrastructure_cost_factor * x
end

harmless_o2_threshold =
   harmless_low_o2_percentage / earth_o2_percentage
function effective_o2(planet)
   return planet.o2_co2_suitability * planet.atmospheric_pressure
end

planet_effects = {
   --[[ example effect
   some_name = {
     value = <a number, or a function that takes a planet and returns a number>,
     apply = function(self, planet)
       <Add logic here to change some property of 'planet'.
          Example: max_pop_mul(self.value, planet)
        Note that some effects (see: infra_cost) don't store value, but have it
        passed in.  This comes from planet_effect_t::value on the C+ side.>
     end
   },
   ]]

   -- general
   uninhab_non_rocky_planet = {
      value = growth_uninhabitable,
      apply = function(self, planet)
         growth_factor_add(planet, self.value)
      end
   },
   only_equatorial_band_habitable = {
      value = only_equatorial_region_habitable_factor,
      apply = function(self, planet)
         max_pop_mul(planet, self.value)
      end
   },
   habs_and_masks_required = {
      value = habs_and_masks_habitable_factor,
      apply = function(self, planet)
         max_pop_mul(planet, self.value)
      end
   },
   habs_and_suits_required = {
      value = habs_and_suits_habitable_factor,
      apply = function(self, planet)
         max_pop_mul(planet, self.value)
      end
   },
   infra_cost = {
      apply = function(self, planet, value)
         infra_mul(planet, value)
      end
   },

   -- seasons
   no_seasons = {
      value = 0.05,
      apply = function(self, planet)
         growth_factor_add(planet, self.value)
      end
   },
   mild_seasons = {
      value = 0.025,
      apply = function(self, planet)
         growth_factor_add(planet, self.value)
      end
   },
   short_seasons = {
      value = function(planet)
         return 0.025 * (1 - planet.orbital_period_y)
      end,
      apply = function(self, planet)
         growth_factor_add(planet, self.value)
      end
   },
   long_seasons = {
      value = function(planet)
         return -0.05 * planet.orbital_period_y
      end,
      apply = function(self, planet)
         growth_factor_add(planet, self.value)
      end
   },
   intense_seasons = {
      value = -0.05,
      apply = function(self, planet)
         growth_factor_add(planet, self.value)
      end
   },

   -- day length
   short_days = {
      value = function(planet)
         return -24.0 / planet.day_h * 0.1
      end,
      apply = function(self, planet)
         growth_factor_add(planet, self.value)
      end
   },
   long_days = {
      value = function(planet)
         return -math.min((planet.day_h - 24.0) * 0.01, 0.1)
      end,
      apply = function(self, planet)
         growth_factor_add(planet, self.value)
      end
   },
   very_long_days = {
      value = function(planet)
         return -0.1 - math.min((planet.day_h - 48.0) * 0.01, 0.1)
      end,
      apply = function(self, planet)
         growth_factor_add(planet, self.value)
      end
   },

   -- O2/CO2 suitability
   poor_o2_co2_suitab = {
      value = function(planet)
         return -(harmless_o2_threshold - effective_o2(planet)) * 0.25
      end,
      apply = function(self, planet)
         growth_factor_add(planet, self.value)
      end
   },
   very_poor_o2_co2_suitab = {
      value = function(planet)
         return -(harmless_o2_threshold - effective_o2(planet)) * 0.25
      end,
      apply = function(self, planet)
         growth_factor_add(planet, self.value)
      end
   },
   marginal_o2_co2_suitab = {
      value = function(planet)
         return -(harmless_o2_threshold - effective_o2(planet)) * 0.25
      end,
      apply = function(self, planet)
         growth_factor_add(planet, self.value)
      end
   },
   insufficient_o2_co2_suitab = {
      value = habs_and_suits_growth_modifier,
      apply = function(self, planet)
         growth_factor_add(planet, self.value)
      end
   },

   -- atmospheric pressure
   high_press_n2_narcosis = {
      value = habs_and_suits_growth_modifier,
      apply = function(self, planet)
         growth_factor_add(planet, self.value)
      end
   },
   very_high_press_o2_toxicity = {
      value = habs_and_suits_growth_modifier,
      apply = function(self, planet)
         growth_factor_add(planet, self.value)
      end
   },

   -- magnetosphere
   very_weak_magneto = {
      value = habs_and_suits_growth_modifier,
      apply = function(self, planet)
         growth_factor_add(planet, self.value)
      end
   },
   weak_magneto = {
      value = function(planet)
         return -(1 - planet.magnetosphere_strength)
      end,
      apply = function(self, planet)
         growth_factor_add(planet, self.value)
      end
   },
   strong_magneto = {
      value = function(planet)
         return (planet.magnetosphere_strength - 1.1) * 0.02
      end,
      apply = function(self, planet)
         growth_factor_add(planet, self.value)
      end
   },

   -- temperature
   extremely_cold_avg_surface_temp = {
      value = habs_and_suits_growth_modifier,
      apply = function(self, planet)
         growth_factor_add(planet, self.value)
      end
   },
   very_cold_avg_surface_temp = {
      value = function(planet)
         return -(earth_temperature_k - 11 - planet.surface_temperature_k) * 0.03
      end,
      apply = function(self, planet)
         growth_factor_add(planet, self.value)
      end
   },
   cold_avg_surface_temp = {
      value = function(planet)
         return -(earth_temperature_k - 11 - planet.surface_temperature_k) * 0.01
      end,
      apply = function(self, planet)
         growth_factor_add(planet, self.value)
      end
   },
   hot_avg_surface_temp = {
      value = function(planet)
         return -(planet.surface_temperature_k - (earth_temperature_k + 11)) * 0.01
      end,
      apply = function(self, planet)
         growth_factor_add(planet, self.value)
      end
   },
   very_hot_avg_surface_temp = {
      value = function(planet)
         return -(planet.surface_temperature_k - (earth_temperature_k + 11)) * 0.03
      end,
      apply = function(self, planet)
         growth_factor_add(planet, self.value)
      end
   },
   extremely_hot_avg_surface_temp = {
      value = habs_and_suits_growth_modifier,
      apply = function(self, planet)
         growth_factor_add(planet, self.value)
      end
   },
   uninhabitably_hot_avg_surface_temp = {
      value = growth_uninhabitable,
      apply = function(self, planet)
         growth_factor_add(planet, self.value)
      end
   },

   -- gravity
   very_low_grav = {
      value = -0.2,
      apply = function(self, planet)
         growth_factor_add(planet, self.value)
      end
   },
   low_grav = {
      value = function(planet)
         return -0.1 * (1 - planet.gravity_g)
      end,
      apply = function(self, planet)
         growth_factor_add(planet, self.value)
      end
   },
   high_grav = {
      value = function(planet)
         return 0.1 * (planet.gravity_g - 1.1)
      end,
      apply = function(self, planet)
         growth_factor_add(planet, self.value)
      end
   },
   very_high_grav = {
      value = function(planet)
         return -(planet.gravity_g - 1.3)
      end,
      apply = function(self, planet)
         growth_factor_add(planet, self.value)
      end
   }
}
