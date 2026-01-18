-- TODO: To constants?
propulsion_force_per_level = 100
mass_moved_through_subspace_per_unit_fuel = 100

function unit_equipment_points(design)
   return design.propulsion + design.weapons + design.shields +
      design.detection + design.stealth + design.fighters
end

function unit_crew(design)
   local shipwide_crew = design.hull * crew_required_per_hull_point
   local equipment_crew =
      unit_equipment_points(design) * crew_required_per_equipment_point
   return shipwide_crew + equipment_crew
end

function unit_ideal_nonequipment_space(design)
   return design.hull -
      space_required_per_equipment_point * unit_equipment_points(design)
end

function unit_nonequipment_space(design)
   return design.hull -
      design.propulsion_space -
      design.weapons_space -
      design.shields_space -
      design.detection_space -
      design.stealth_space
end

function unit_crew_space(crew)
   return to_integer(crew / 1000 * space_required_per_1k_crew + 0.5)
end

function unit_ideal_unused_space(design)
   local storage_space = design.fuel + design.water + design.supplies +
      design.rounds + design.missiles + design.fighters + design.cargo
   return unit_ideal_nonequipment_space(design) -
      storage_space - unit_crew_space(unit_crew(design))
end

function unit_unused_space(design)
   local storage_space = design.fuel + design.water + design.supplies +
      design.rounds + design.missiles + design.fighters + design.cargo
   return unit_nonequipment_space(design) -
      storage_space - unit_crew_space(unit_crew(design))
end

function hull_cost(points)
   local retval = cost_t.new()
   retval.money_cost = points
   retval.metal_cost = points
   retval.energy_cost = points
   retval.water_cost = points * 0.01
   return retval
end

function armor_cost(points)
   local retval = cost_t.new()
   retval.money_cost = points * 2
   retval.metal_cost = points * 2
   retval.energy_cost = points * 2
   retval.water_cost = points * 0.01
   return retval
end

function propulsion_cost(points)
   local retval = cost_t.new()
   retval.money_cost = points
   retval.metal_cost = points
   retval.energy_cost = points
   retval.fuel_cost = points * 0.01
   retval.water_cost = points * 0.001
   return retval
end

function weapons_cost(points)
   local retval = cost_t.new()
   retval.money_cost = points * 1.5
   retval.metal_cost = points * 0.2
   retval.energy_cost = points * 0.5
   retval.water_cost = points * 0.001
   return retval
end

function shields_cost(points)
   local retval = cost_t.new()
   retval.money_cost = points * 2.0
   retval.metal_cost = points * 0.2
   retval.energy_cost = points
   retval.water_cost = points * 0.001
   return retval
end

function detection_cost(points)
   local retval = cost_t.new()
   retval.money_cost = points * 0.1
   retval.metal_cost = points * 0.001
   retval.energy_cost = points * 0.1
   return retval
end

function stealth_cost(points)
   local retval = cost_t.new()
   retval.money_cost = points
   retval.metal_cost = points * 0.001
   retval.energy_cost = points
   return retval
end

function fighters_cost(points)
   local retval = cost_t.new()
   retval.money_cost = points * 0.1
   retval.metal_cost = points * 0.01
   retval.energy_cost = points * 0.01
   return retval
end

-- TODO: Add crew to cost_t; return it as part of these costs.
function unit_ideal_cost(design)
   return hull_cost(design.hull) +
      armor_cost(design.armor) +
      propulsion_cost(design.propulsion * 10) +
      weapons_cost(design.weapons * 10) +
      shields_cost(design.shields * 10) +
      detection_cost(design.detection * 10) +
      stealth_cost(design.stealth * 10) +
      fighters_cost(design.fighters)
end

function unit_cost(design)
   return hull_cost(design.hull) +
      armor_cost(design.armor) +
      propulsion_cost(design.propulsion_space) +
      weapons_cost(design.weapons_space) +
      shields_cost(design.shields_space) +
      detection_cost(design.detection_space) +
      stealth_cost(design.stealth_space) +
      fighters_cost(design.fighters_space)
end

function unit_ideal_mass(design)
   return unit_ideal_cost(design).metal_cost
end

function unit_mass(design)
   return unit_cost(design).metal_cost
end

-- world units per month
function unit_subspace_speed(design)
   return design.propulsion
end

-- world units
function unit_ideal_subspace_range(design)
   return design.fuel * fuel_efficiency(nation) /
      (unit_ideal_mass(design) / mass_moved_through_subspace_per_unit_fuel)
end

-- world units
function unit_subspace_range(design)
   return design.fuel * fuel_efficiency(nation) /
      (unit_mass(design) / mass_moved_through_subspace_per_unit_fuel)
end

-- value is in Gs
function unit_ideal_max_acceleration(design)
   return design.propulsion * propulsion_force_per_level / unit_ideal_mass(design)
end

-- value is in Gs
function unit_max_acceleration(design)
   return design.effective_propulsion * propulsion_force_per_level / unit_mass(design)
end

-- value is in Gs
function unit_ideal_sustained_acceleration(design, nation)
   return math.min(unit_ideal_max_acceleration(design),
                   max_sustained_acceleration(nation))
end

-- value is in Gs
function unit_sustained_acceleration(design, nation)
   return math.min(unit_max_acceleration(design),
                   max_sustained_acceleration(nation))
end

function unit_days_at_sustained_acceleration(design, nation)
   return design.fuel * fuel_efficiency(nation) / design.propulsion
end

function unit_months_of_water(design)
   return design.water / (unit_crew(design) / 1000)
end

function unit_months_of_supplies(design)
   return design.supplies / (unit_crew(design) / 1000)
end

function unit_pd_volleys(design)
   return design.rounds * pd_volleys_per_unit_storage
end

function unit_missile_volleys(design)
   return design.missiles * missile_volleys_per_unit_storage
end

-- the design novelty score for a single ship equipment level, based on how
-- many times it was ued in previous design (first velua is for 0 times, not
-- oncce)
iteration_design_novelty = {1.0, 0.5, 0.25}
