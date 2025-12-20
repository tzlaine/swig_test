function eval_planet_effect_expr(planet, expr)
   if type(expr) == 'string' then
      local env = { planet = planet, math = math }
      local func, err = load('return '..expr, nil, 't', env)
      return func()
   else
      return expr
   end
end

function growth_factor_add(planet, expr)
   x = eval_planet_effect_expr(planet, expr)
   planet.growth_factor = planet.growth_factor + x
end
function max_pop_mul(planet, expr)
   x = eval_planet_effect_expr(planet, expr)
   planet.max_population = planet.max_population * x
end
function infra_mul(planet, expr)
   x = eval_planet_effect_expr(planet, expr)
   planet.infrastructure_cost_factor = planet.infrastructure_cost_factor * x
end

harmless_low_o2_percentage = 19.5
earth_o2_percentage = 20.9
harmless_o2_threshold =
   harmless_low_o2_percentage / earth_o2_percentage
function effective_o2(planet)
   return planet.o2_co2_suitability * planet.atmopsheric_pressure
end

planet_effects = {
   --[[ example effect
   some_name = {
     expr = <a number, or a string that can be evaluated in terms of planet>,
     apply = function(self, planet)
       <Add logic here to change some property of 'planet'.
          Example: max_pop_mul(self.expr, planet)
        Note that some effects (see: infra_cost) don't store expr, but have it
        passed in.  This comes from planet_effect_t::value on the C+ side.>
       <Return true here if the effect is temporary, and just ended.>
     end
   },
   ]]

   -- general
   uninhab_non_rocky_planet = {
      expr = growth_uninhabitable,
      apply = function(self, planet)
         growth_factor_add(planet, self.expr)
      end
   },
   only_equatorial_band_habitable = {
      expr = only_equatorial_region_habitable_factor,
      apply = function(self, planet)
         max_pop_mul(planet, self.expr)
      end
   },
   habs_and_masks_required = {
      expr = habs_and_masks_habitable_factor,
      apply = function(self, planet)
         max_pop_mul(planet, self.expr)
      end
   },
   habs_and_suits_required = {
      expr = habs_and_suits_habitable_factor,
      apply = function(self, planet)
         max_pop_mul(planet, self.expr)
      end
   },
   infra_cost = {
      apply = function(self, planet, expr)
         infra_mul(planet, expr)
      end
   },

   -- seasons
   no_seasons = {
      expr = 0.05,
      apply = function(self, planet)
         growth_factor_add(planet, self.expr)
      end
   },
   mild_seasons = {
      expr = 0.025,
      apply = function(self, planet)
         growth_factor_add(planet, self.expr)
      end
   },
   short_seasons = {
      expr = '0.025 * (1 - planet.orbital_period_y)',
      apply = function(self, planet)
         growth_factor_add(planet, self.expr)
      end
   },
   long_seasons = {
      expr = '-0.05 * planet.orbital_period_y',
      apply = function(self, planet)
         growth_factor_add(planet, self.expr)
      end
   },
   intense_seasons = {
      expr = -0.05,
      apply = function(self, planet)
         growth_factor_add(planet, self.expr)
      end
   },

   -- day length
   short_days = {
      expr = '-24.0 / planet.day_h * 0.1',
      apply = function(self, planet)
         growth_factor_add(planet, self.expr)
      end
   },
   long_days = {
      expr = '-math.min((planet.day_h - 24.0) * 0.01, 0.1)',
      apply = function(self, planet)
         growth_factor_add(planet, self.expr)
      end
   },
   very_long_days = {
      expr = '-0.1 - math.min((planet.day_h - 48.0) * 0.01, 0.1)',
      apply = function(self, planet)
         growth_factor_add(planet, self.expr)
      end
   },

   -- O2/CO2 suitability
   poor_o2_co2_suitab = {
      expr = '-(harmless_o2_threshold - effective_o2(planet)) * 0.25',
      apply = function(self, planet)
         growth_factor_add(planet, self.expr)
      end
   },
   very_poor_o2_co2_suitab = {
      expr = '-(harmless_o2_threshold - effective_o2(planet)) * 0.25',
      apply = function(self, planet)
         growth_factor_add(planet, self.expr)
      end
   },
   marginal_o2_co2_suitab = {
      expr = '-(harmless_o2_threshold - effective_o2(planet)) * 0.25',
      apply = function(self, planet)
         growth_factor_add(planet, self.expr)
      end
   },
   insufficient_o2_co2_suitab = {
      expr = habs_and_suits_growth_modifier,
      apply = function(self, planet)
         growth_factor_add(planet, self.expr)
      end
   },

   -- atmospheric pressure
   high_press_n2_narcosis = {
      expr = habs_and_suits_growth_modifier,
      apply = function(self, planet)
         growth_factor_add(planet, self.expr)
      end
   },
   very_high_press_o2_toxicity = {
      expr = habs_and_suits_growth_modifier,
      apply = function(self, planet)
         growth_factor_add(planet, self.expr)
      end
   },

   -- magnetosphere
   very_weak_magneto = {
      expr = habs_and_suits_growth_modifier,
      apply = function(self, planet)
         growth_factor_add(planet, self.expr)
      end
   },
   weak_magneto = {
      expr = '-(1 - planet.magnetosphere_strength)',
      apply = function(self, planet)
         growth_factor_add(planet, self.expr)
      end
   },
   strong_magneto = {
      expr = '(planet.magnetosphere_strength - 1.1) * 0.02',
      apply = function(self, planet)
         growth_factor_add(planet, self.expr)
      end
   },

   -- temperature
   extremely_cold_avg_surface_temp = {
      expr = habs_and_suits_growth_modifier,
      apply = function(self, planet)
         growth_factor_add(planet, self.expr)
      end
   },
   very_cold_avg_surface_temp = {
      expr = '-(earth_temperature_k - 11 - planet.surface_temperature_k) * 0.03',
      apply = function(self, planet)
         growth_factor_add(planet, self.expr)
      end
   },
   cold_avg_surface_temp = {
      expr = '-(earth_temperature_k - 11 - planet.surface_temperature_k) * 0.01',
      apply = function(self, planet)
         growth_factor_add(planet, self.expr)
      end
   },
   hot_avg_surface_temp = {
      expr = '-(planet.surface_temperature_k - (earth_temperature_k + 11)) * 0.01',
      apply = function(self, planet)
         growth_factor_add(planet, self.expr)
      end
   },
   very_hot_avg_surface_temp = {
      expr = '-(planet.surface_temperature_k - (earth_temperature_k + 11)) * 0.03',
      apply = function(self, planet)
         growth_factor_add(planet, self.expr)
      end
   },
   extremely_hot_avg_surface_temp = {
      expr = habs_and_suits_growth_modifier,
      apply = function(self, planet)
         growth_factor_add(planet, self.expr)
      end
   },
   uninhabitably_hot_avg_surface_temp = {
      expr = growth_uninhabitable,
      apply = function(self, planet)
         growth_factor_add(planet, self.expr)
      end
   },

   -- gravity
   very_low_grav = {
      expr = -0.2,
      apply = function(self, planet)
         growth_factor_add(planet, self.expr)
      end
   },
   low_grav = {
      expr = '-0.1 * (1 - planet.gravity_g)',
      apply = function(self, planet)
         growth_factor_add(planet, self.expr)
      end
   },
   high_grav = {
      expr = '0.1 * (planet.gravity_g - 1.1)',
      apply = function(self, planet)
         growth_factor_add(planet, self.expr)
      end
   },
   very_high_grav = {
      expr = '-(planet.gravity_g - 1.3)',
      apply = function(self, planet)
         growth_factor_add(planet, self.expr)
      end
   }
}
