-- TODO: construction_months()

-- TODO: roll_construction_month()

function create_unit(design)
   local retval = unit_t.new()
   retval.id = design.id
   retval.hull = design.hull
   retval.armor = design.armor
   retval.propulsion = design.effective_propulsion
   retval.weapons = design.effective_weapons
   retval.shields = design.effective_shields
   retval.detection = design.effective_detection
   retval.stealth = design.effective_stealth
   retval.fuel = 0
   retval.water = 0
   retval.supplies = 0
   retval.rounds = 0
   retval.missiles = 0
   retval.fighters = 0
   retval.organization = 0
   retval.combat_experience = 0
   retval.crew = 0
   return retval
end

function unit_day_update(unit, organization_gain)
   unit.organization = math.max(unit.organization + organization_gain, 100)
end

function unit_month_update(unit)
   unit.water = math.max(0, unit.water - unit.crew / 1000)
   unit.supplies = math.max(0, unit.supplies - unit.crew / 1000)
end

function resupply_unit(unit, design, settlement)
   local transferred = 0

   transferred = math.min(design.fuel - unit.fuel, settlement.fuel.stockpile)
   unit.fuel = unit.fuel + transferred
   settlement.fuel.stockpile = settlement.fuel.stockpile - transferred

   transferred = math.min(design.water - unit.water, settlement.water.stockpile)
   unit.water = unit.water + transferred
   settlement.water.stockpile = settlement.water.stockpile - transferred

   transferred =
      math.min(design.supplies - unit.supplies, settlement.supplies.stockpile)
   unit.supplies = unit.supplies + transferred
   settlement.supplies.stockpile = settlement.supplies.stockpile - transferred

   transferred = math.min(
      design.rounds * pd_volleys_per_unit_storage - unit.rounds,
      settlement.rounds.stockpile)
   unit.rounds = unit.rounds + transferred
   settlement.rounds.stockpile = settlement.rounds.stockpile - transferred

   transferred = math.min(
      design.missiles * missile_volleys_per_unit_storage - unit.missiles,
      settlement.missiles.stockpile)
   unit.missiles = unit.missiles + transferred
   settlement.missiles.stockpile = settlement.missiles.stockpile - transferred

   transferred =
      math.min(design.fighters - unit.fighters, settlement.fighters.stockpile)
   unit.fighters = unit.fighters + transferred
   settlement.fighters.stockpile = settlement.fighters.stockpile - transferred

   transferred =
      math.min(design.crew - unit.crew, settlement.crewpower.stockpile)
   local fresh_recruits = design.crew - unit.crew - transferred
   local weighted_xp_total =
      unit.crew * unit.combat_experience + -- crew at the unit's xp level
      transferred * 0 +                    -- fully-trained crew with 0 xp
      fresh_recruits * -100                -- randos that signed up today
   unit.crew = unit.crew + transferred + fresh_recruits
   settlement.crewpower.stockpile = settlement.crewpower.stockpile - transferred
   unit.combat_experience = weighted_xp_total / unit.crew
end

function repair_cost(unit, design)
   local retval = hull_cost(design.hull - unit.hull)
   for i = 1, #unit.hit_table do
      if unit.hit_table[i] == hit_destroyed then
         if design[i] == hit_propulsion then
            retval = retval + propulsion_cost(1)
         elseif design[i] == hit_weapons then
            retval = retval + weapons_cost(1)
         elseif design[i] == hit_shields then
            retval = retval + shields_cost(1)
         elseif design[i] == hit_detection then
            retval = retval + detection_cost(1)
         elseif design[i] == hit_stealth then
            retval = retval + stealth_cost(1)
         end
      end
   end
   return retval
end

-- TODO: repair_months()

-- TODO: roll_repair_month()
