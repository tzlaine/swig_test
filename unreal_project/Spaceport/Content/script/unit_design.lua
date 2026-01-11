-- TODO: To constants?
crew_required_per_hull_point = 5
space_required_per_equipment_point = 10

function unit_equipment_points(design)
   return design.propulsion + design.weapons + design.shields +
      design.detection + design.stealth + design.fighters
end

function unit_crew(design)
   local shipwide_crew = design.hull * crew_required_per_hull_point
   local equipment_crew = unit_equipment_points(design)
   return shipwide_crew + equipment_crew
end

function unit_nonequipment_space(design)
   return design.hull -
      space_required_per_equipment_point * unit_equipment_points(design)
end

function unit_crew_space(crew)
   return to_integer(crew / 1000.0 * 3.0 + 0.5)
end

function unit_unused_space(design)
   local storage_space = design.fuel + design.water + design.supplies +
      design.rounds + design.missiles + design.cargo
   return unit_nonequipment_space(design) -
      storage_space - unit_crew_space(unit_crew(design))
end

function unit_mass(design)
   return design.hull + design.armor
end

-- world units per month
function unit_subspace_speed(design)
   return design.propulsion
end

-- value is in Gs
function unit_max_acceleration(design)
   return 100 * design.propulsion / unit_mass(design)
end

function unit_cost(design)
   return -10 -- TODO
end
