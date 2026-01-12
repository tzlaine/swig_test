function max_hull_space(nation)
   return nation.construction_tech * 100
end

-- A value of 0 means that fighters are not available at all.
function base_fighter_factor_strength(nation)
   local min_propulsion = 10
   local min_weapons = 10
   local from_propulsion = nation.propulsion_tech - min_propulsion
   local from_weapons = nation.weapons_tech - min_weapons
   if from_propulsion < 0 or from_weapons < 0 then
      return 0
   end
   return 10 + from_propulsion + from_weapons
end

-- in Gs
function max_sustained_acceleration(nation)
   -- TODO: tech should increase this
   return 1.5
end

function fuel_efficiency(nation)
   -- TODO: tech should increase this
   return 1
end
