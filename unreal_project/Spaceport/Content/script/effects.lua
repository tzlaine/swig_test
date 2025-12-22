function effect_value(obj, value)
   if type(value) == 'function' then
      return value(obj)
   else
      return value
   end
end

harmless_o2_threshold =
   harmless_low_o2_percentage / earth_o2_percentage
function effective_o2(planet)
   return planet.o2_co2_suitability * planet.atmospheric_pressure
end

planet_effects = {
   --[[ Example effect.  You must always have a target field, but the apply
        field is only necessary if the given target does not already have an
        implementation.

   some_name = {
      value = <a number, or a function that takes a planet and returns a number>,
      target = <target name>, -- 'foo' for the affected property planet.foo
      apply = function(self, planet) -- or function(self, planet, value)
         <Add logic here to change some property of 'planet'.
            Example: max_pop_mul(self.value, planet)
         Note that some effects (see: infra_cost) don't store value, but have it
         passed in.  This is taken as a third argument, and is taken from
         planet_effect_t::value on the C++ side.>
      end
   },
   ]]

   -- general
   uninhab_non_rocky_planet = {
      value = growth_uninhabitable,
      target = 'growth_factor'
   },
   only_equatorial_band_habitable = {
      value = only_equatorial_region_habitable_factor,
      target = 'max_population'
   },
   habs_and_masks_required = {
      value = habs_and_masks_habitable_factor,
      target = 'max_population'
   },
   habs_and_suits_required = {
      value = habs_and_suits_habitable_factor,
      target = 'max_population'
   },
   infra_cost = {
      -- value comes from the .value member of the C++ planet_effect_t struct.
      target = 'infrastructure_cost'
   },

   -- seasons
   no_seasons = {
      value = 0.05,
      target = 'growth_factor'
   },
   mild_seasons = {
      value = 0.025,
      target = 'growth_factor'
   },
   short_seasons = {
      value = function(planet)
         return 0.025 * (1 - planet.orbital_period_y)
      end,
      target = 'growth_factor'
   },
   long_seasons = {
      value = function(planet)
         return -0.05 * planet.orbital_period_y
      end,
      target = 'growth_factor'
   },
   intense_seasons = {
      value = -0.05,
      target = 'growth_factor'
   },

   -- day length
   short_days = {
      value = function(planet)
         return -24.0 / planet.day_h * 0.1
      end,
      target = 'growth_factor'
   },
   long_days = {
      value = function(planet)
         return -math.min((planet.day_h - 24.0) * 0.01, 0.1)
      end,
      target = 'growth_factor'
   },
   very_long_days = {
      value = function(planet)
         return -0.1 - math.min((planet.day_h - 48.0) * 0.01, 0.1)
      end,
      target = 'growth_factor'
   },

   -- O2/CO2 suitability
   poor_o2_co2_suitab = {
      value = function(planet)
         return -(harmless_o2_threshold - effective_o2(planet)) * 0.25
      end,
      target = 'growth_factor'
   },
   very_poor_o2_co2_suitab = {
      value = function(planet)
         return -(harmless_o2_threshold - effective_o2(planet)) * 0.25
      end,
      target = 'growth_factor'
   },
   marginal_o2_co2_suitab = {
      value = function(planet)
         return -(harmless_o2_threshold - effective_o2(planet)) * 0.25
      end,
      target = 'growth_factor'
   },
   insufficient_o2_co2_suitab = {
      value = habs_and_suits_growth_modifier,
      target = 'growth_factor'
   },

   -- atmospheric pressure
   high_press_n2_narcosis = {
      value = habs_and_suits_growth_modifier,
      target = 'growth_factor'
   },
   very_high_press_o2_toxicity = {
      value = habs_and_suits_growth_modifier,
      target = 'growth_factor'
   },

   -- magnetosphere
   very_weak_magneto = {
      value = habs_and_suits_growth_modifier,
      target = 'growth_factor'
   },
   weak_magneto = {
      value = function(planet)
         return -(1 - planet.magnetosphere_strength)
      end,
      target = 'growth_factor'
   },
   strong_magneto = {
      value = function(planet)
         return (planet.magnetosphere_strength - 1.1) * 0.02
      end,
      target = 'growth_factor'
   },

   -- temperature
   extremely_cold_avg_surface_temp = {
      value = habs_and_suits_growth_modifier,
      target = 'growth_factor'
   },
   very_cold_avg_surface_temp = {
      value = function(planet)
         return -(earth_temperature_k - 11 - planet.surface_temperature_k) * 0.03
      end,
      target = 'growth_factor'
   },
   cold_avg_surface_temp = {
      value = function(planet)
         return -(earth_temperature_k - 11 - planet.surface_temperature_k) * 0.01
      end,
      target = 'growth_factor'
   },
   hot_avg_surface_temp = {
      value = function(planet)
         return -(planet.surface_temperature_k - (earth_temperature_k + 11)) * 0.01
      end,
      target = 'growth_factor'
   },
   very_hot_avg_surface_temp = {
      value = function(planet)
         return -(planet.surface_temperature_k - (earth_temperature_k + 11)) * 0.03
      end,
      target = 'growth_factor'
   },
   extremely_hot_avg_surface_temp = {
      value = habs_and_suits_growth_modifier,
      target = 'growth_factor'
   },
   uninhabitably_hot_avg_surface_temp = {
      value = growth_uninhabitable,
      target = 'growth_factor'
   },

   -- gravity
   very_low_grav = {
      value = -0.2,
      target = 'growth_factor'
   },
   low_grav = {
      value = function(planet)
         return -0.1 * (1 - planet.gravity_g)
      end,
      target = 'growth_factor'
   },
   high_grav = {
      value = function(planet)
         return 0.1 * (planet.gravity_g - 1.1)
      end,
      target = 'growth_factor'
   },
   very_high_grav = {
      value = function(planet)
         return -(planet.gravity_g - 1.3)
      end,
      target = 'growth_factor'
   }
}
