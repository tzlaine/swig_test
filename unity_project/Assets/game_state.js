#pragma strict

import System.IO;
import RadicalRoutineExtensions;
import System.Runtime.InteropServices;
import model_plugin;

@script SerializeAll

@DoNotSerialize
var game_data_ : game_data;
@DoNotSerialize
var place_fleets_ui : place_fleets;

private var in_setup_ : boolean = true;
private var turn_ : int;
private var player_nations_ : Dictionary.<String, boolean>;
private var game_guid_ : String;

@DoNotSerialize
private static var this_ : game_state;

@DoNotSerialize
private var save_pending = false;

function save_async ()
{ save_pending = true; }

function turn ()
{ return turn_; }

private function turn_to_str (t : int) : String
{
    var retval : String = t / 10 == 0 ? 'Spring' : 'Fall';
    retval += ', Y' + Mathf.FloorToInt(t / 10.0);
    return retval;
}

private function save_description () : String
{
    var retval : String = game_data_.scenario().name;
    if (in_setup_)
        retval += ' - Initial setup';
    else
        retval += ' - ' + turn_to_str(turn_);
    return retval;
}

private function save ()
{
    var info = new DirectoryInfo(Application.persistentDataPath);
    var files = info.GetFiles();
    var matching_guid_saves = new Array();
    for (file in files) {
        var filename : String = file.ToString();
        if (filename.StartsWith(game_guid_))
            matching_guid_saves.Push(filename);
    }

    var save_filename : String = game_guid_;
    JSONLevelSerializer.SerializeLevelToFile(game_guid_ + save_description());

    for (match in matching_guid_saves) {
        File.Delete(match);
    }
}

function clear ()
{
    // TODO
}

private static function initial_setup ()
{
    var order = this_.game_data_.scenario().setup_order;
    for (var nation in order) {
        // TODO: Hotseat only for now.
        //if (this_.player_nations_[nation]) {
        if (true) {
            this_.place_fleets_ui.set_up(nation);
            while (!this_.place_fleets_ui.done) {
                yield WaitForSeconds(0.1);
            }
            this_.save_async();
        } else {
            // TODO: wait for other player to set up this nation.
        }
    }
}

private static function determine_supply ()
{
    Debug.Log('determine_supply()');

    var scenario = this_.game_data_.scenario();

    var nation_teams = new int[this_.game_data_.nations_by_id.Count];
    var capitals = new int[this_.game_data_.nations_by_id.Count];
    var nation_offmap_areas = new int[this_.game_data_.nations_by_id.Count];
    for (var k = 0; k < nation_teams.Length; ++k) {
        nation_teams[k] = -1;
        capitals[k] = -1;
        nation_offmap_areas[k] = -1;
    }

/*  // TODO
    var team_id = 0;
    for (team in scenario.teams) {
        for (member in team.Value) {
            var id = this_.game_data_.id(member);
            nation_teams[id] = team_id;
            for (cap in this_.game_data_.nation(member).Capital) {
                if (cap.Value.features.Contains('CAP'))
                    capitals[id] = cap.Value.hc.GetHashCode();
            }
        }
        ++team_id;
    }

    var m = this_.game_data_.map();
    var hexes = new supply_check_hex_t[m.width * m.height];
    var i = 0;
    for (h in m.hexes) {
        var sh = new supply_check_hex_t();
        sh.owner_id = h.owner_id;
        for (n in h.units) {
            var owner_team = nation_teams[this_.game_data_.id(n.Key)];
            for (u in n.Value) {
                if (true) { // TODO: Handle tugs with supply mission.
                    sh.ship |= 1 << sh.owner_id;
                    if (false) // TODO: Handle tugs with supply mission.
                        sh.supply_tug |= 1 << sh.owner_id;
                } else {
                    sh.nonship_unit |= 1 << sh.owner_id;
                    if (false) // TODO: Handle CONVOYs.
                        sh.convoy |= 1 << sh.owner_id;
                }
            }
        }

        // TODO: Account for ownership of these things.
        // TODO: Use .zones instead.
        if (h.feature == 'SB') {
            sh.SB |= 1 << sh.owner_id;
            if (true) // TODO: Handle fighters or PFs
                sh.base_with_fighters |= 1 << sh.owner_id;
        } else if (h.feature == 'BATS') {
            sh.BATS |= 1 << sh.owner_id;
            if (true) // TODO: Handle fighters or PFs
                sh.base_with_fighters |= 1 << sh.owner_id;
        } else if (h.feature == 'MIN' || h.feature == 'MAJ' || h.feature == 'capitol') {
            sh.planet |= 1 << sh.owner_id;
        }
        hexes[i++] = sh;
    }

    var offmap_border_hexes =
        new int[this_.game_data_.nations_by_id.Count * model_plugin.max_offmap_border_hexes];
    for (i = 0; i < offmap_border_hexes.Length; ++i) {
        offmap_border_hexes[i] = -1;
    }

    for (offmap in m.offmap_areas) {
        var id_ = this_.game_data_.id(offmap.Key);
        var index = id_ * model_plugin.max_offmap_border_hexes;
        for (hc in offmap.Value.adjacent_hexes) {
            offmap_border_hexes[index] = hc.GetHashCode();
            ++index;
        }
    }

    var supply_ : System.IntPtr = model_plugin.determine_supply(
        m.width, m.height,
        hexes,
        this_.game_data_.nation('NZ').id,
        nation_teams.Length,
        nation_teams,
        capitals,
        model_plugin.max_offmap_border_hexes,
        offmap_border_hexes
    );

    var supply = new int[m.width * m.height];
    Marshal.Copy(supply_, supply, 0, m.width * m.height);

    var color : Vector4 = Vector4(1, 0.5, 0.5, 1); // = TODO
    var supplied_hexes_by_owner_tmp : Dictionary.<int, Array> = new Dictionary.<int, Array>();
    var hexes_by_grid_tmp : Dictionary.<int, Array> = new Dictionary.<int, Array>();
    var grids_by_owner_tmp : Dictionary.<int, Array> = new Dictionary.<int, Array>();
    for (i = 0; i < supply.Length; ++i) {
        var hc : hex_coord = hex_coord(i % m.width, i / m.width);
        var s : int = supply[i];

        for (var owner_id_bit : int = 8; owner_id_bit < 24; ++owner_id_bit) {
            var owner_id : int = owner_id_bit - 8;
            if (s & (1 << owner_id_bit)) {
                if (!supplied_hexes_by_owner_tmp.ContainsKey(owner_id))
                    supplied_hexes_by_owner_tmp[owner_id] = Array();
                supplied_hexes_by_owner_tmp[owner_id].Push(hc);
            }
        }

        var grid : int = s & 0xff;
        if (!hexes_by_grid_tmp.ContainsKey(grid))
            hexes_by_grid_tmp[grid] = new Array();
        hexes_by_grid_tmp[grid].Push(hc);

        var hex : hex_t = this_.game_data_.map().hex(hc);
        owner_id = hex.owner_id;
        if (!grids_by_owner_tmp.ContainsKey(owner_id))
            grids_by_owner_tmp[owner_id] = new Array();
        grids_by_owner_tmp[owner_id].Push(grid);

        // TODO: Record bit 24's value somewhere.
    }

    var supplied_hexes_by_owner : Dictionary.<int, hex_coord[]> = new Dictionary.<int, hex_coord[]>();
    var hexes_by_grid : Dictionary.<int, hex_coord[]> = new Dictionary.<int, hex_coord[]>();
    var grids_by_owner : Dictionary.<int, int[]> = new Dictionary.<int, int[]>();
    for (var kv : KeyValuePair.<int, Array> in supplied_hexes_by_owner_tmp) {
        supplied_hexes_by_owner[kv.Key] = kv.Value.ToBuiltin(hex_coord);
    }
    for (var kv : KeyValuePair.<int, Array> in hexes_by_grid_tmp) {
        hexes_by_grid[kv.Key] = kv.Value.ToBuiltin(hex_coord);
    }
    for (var kv : KeyValuePair.<int, Array> in grids_by_owner_tmp) {
        grids_by_owner[kv.Key] = kv.Value.ToBuiltin(int);
    }
*/

    Debug.Log('did supply determination step');
}

private static function economics (team : String)
{
    yield WaitForSeconds(0.001); // TODO
    /*  PHASE 1: ECONOMICS

        1A1: Base and PDU (or PGB‡) deployments/upgrades begun on a previous
        Phasing Player Turn become operational. TODO

        1A2: Both players conduct first check to evaluate supply status for
        combat (410.22).

        1B: Phasing Player determines the total number of survey points
        generated (505.21).

        1C: Phasing Player determines the total income in Economic Points
        (430.1) and repays deficit spending. Phasing Player may transfer EPs
        from a newly connected Partial Supply Grid to the main grid (413.44).

        1D: Phasing Player removes Reserve markers placed on the previous turn
        (507.1).

        1E: This step is not used in Basic F&E.

        1F: Phasing Player declares missions for tugs and other transports
        (509.32). Some tugs and transports may have their missions assigned or
        change later (509.1).

        1G: Phasing Player designates Expeditionary Fleets (411.7).

        1H: Phasing Player purchases and/or designates support lines for
        specific homeless ships (410.56).
    */
}

private static function repair (team : String)
{
    yield WaitForSeconds(0.001); // TODO
    /*  PHASE 2A: REPAIR

        2A1: Phasing Player evaluates supply status for repairs (410.23). 2A2:
        Phasing Player allocates Economic Points to repair damaged ships/units
        waiting at bases/FRDs (420.2).

        2A3: Phasing Player repairs bases/SIDS (420.6). Tugs and transports
        may be assigned Mission J1 (509.1).

        2A4: Phasing Player flips repaired units back over to full strength
        (420.52).
    */
}

private static function production (team : String)
{
    yield WaitForSeconds(0.001); // TODO
    /*  PHASE 2B: PRODUCTION
        The Phasing Player may complete the following:

        2B1: Activate mothballed units (434.0).

        2B2: This step is not used in Basic F&E.

        2B3: Receive new units and assign site where they were built
        (431.0). Perform ship conversions (433.0). Allocate funds to pgrade
        bases (433.41) and PDUs (433.42). Tugs and transports may be assigned
        missions J2, J3, J4 (509.1).

        2B4: Replace fighters (501.5), PFs (502.43), and GCEs (521.22) if in
        supply; place initial deployment PFs (502.6) if ap- plicable.

        2B5: Convert modular ships at starbases or stellar fortresses‡ to new
        missions (433.432).

        2B6: This sub-step is not used in Basic F&E.

        2B7: Assign missions to new tugs and transports (509.35). Assign tugs
        and transports Mission D (509.1).

        2B8: This sub-step is not used in Basic F&E.

        2B9: Federation player designates replacement squadron locations of
        special fighters (F-14s/F-15s) (502.95).

        2B10: Authorized players conduct battleship construction, selecting
        one of the options under (436.2).

        2B11: Players may scuttle units they control (301.911).
    */
}

private static function operational_movement (team : String)
{
    yield WaitForSeconds(0.001); // TODO
    /*  PHASE 3: OPERATIONS

        PHASE 3A: RAIDS
        This phase is not used in Basic F&E.

        PHASE 3B: OPERATIONAL MOVEMENT

        3B1: Evaluate Operational Movement supply status (410.21). 3B2:
        Phasing Player moves ships and units within limitations of the rules;
        modular ships may be reconfigured by giving up movement and Economic
        Points [Romulan Modular Ships (433.432)]. Certain tug and transport
        missions may be assigned; see (509.1).  Note: Eligible Non-Phasing
        Player’s units may use Reaction Movement during Step 3B2 (205.0).

        3B3-3B5: These sub-steps are not used in Basic F&E.

        3B6: Phasing Player announces and begins deployment of mobile bases
        (510.22) and PDUs (and PGBs‡) as per rule (508.32).
    */
}

private static function reserve_movement (team : String)
{
    yield WaitForSeconds(0.001); // TODO
    /*  PHASE 4: RESERVE MOVEMENT

        4A: Non-Phasing Player can move his designated Reserve Fleets (203.7).

        4B: This step is not used in Basic F&E.
    */
}

private static function combat (team : String)
{
    yield WaitForSeconds(0.001); // TODO
    /*  PHASE 5: COMBAT

        Each hex in which both players have units becomes a Battle
        Hex. Phasing Player selects a Battle Hex and combat within that hex is
        completely resolved (using the steps below); repeat these steps for
        every Battle Hex until all combat is resolved (302.9).

        PHASE 5 - STEP 1: WITHDRAWAL BEFORE COMBAT (302.1)

        5-1A: Non-Phasing Player announces if he will withdraw (302.11). 5-1B:
        Phasing Player announces if he will oppose withdrawal (302.11).

        5-1C: Non-Phasing Player conducts withdrawal [unopposed (302.12);
        opposed (302.13)] using retreat procedures in Phase 5 Step #7 below.

        5-1D: Non-Phasing Player selects flagship of non-withdrawing force;
        this unit MUST serve as the flagship in the first combat round
        (302.133).

        5-1E: Non-Phasing Player’s cloaked ships can try to evade (306.1). If
        the chosen flagship successfully uses cloaked evasion (306.1) in Step
        5-1D, then a new flagship must be chosen from any of the eligible
        units that failed to evade.

        5-1F: If all defending units withdraw from the hex, the Battle Hex is
        resolved. If withdrawing forces contain crippled units, then attacker
        may choose to pursue using Phase 5 Step #8 below. If any opposing
        units remain in the hex then resolve combat using steps below. If
        there is no pursuit, proceed to the next Battle Hex and resolve that.


        PHASE 5 - STEP 2: ACTION WITH BASES (302.2)

        [If no base or base-like unit (302.211) is present, then go to Phase 5 Step 3.]

        5-2A: APPROACH BATTLE

        5-2A1: Attacker may use cloaking devices to avoid the approach battle
        (306.3), if so move to Phase 5 - Step #3 and include the
        base. Attacker may not use (306.2) in this Battle Hex.

        5-2A2: Attacker offers approach battle (mandatory on first round if
        not avoided by use of cloaking devices) (302.22).

        5-2A3: Defender accepts or rejects approach battle; if rejected move
        to Phase 5 - Step #2B, if accepted move to Phase 5 - Step #3 but do
        not include the base.

        5-2A4: Approach battle is fought using combat steps below.

        5-2B: BASE BATTLE

        5-2B1: Attacker exercises option to retreat before combat at base
        (302.23); if attacker retreats then proceed to Phase 5 - Step #7.

        5-2B2: Conduct base battle using combat steps below; see exception
        for Capital Assaults (511.5).

        PHASE 5 - STEP 3: BATTLE FORCE DETERMINATION (302.3)

        5-3A: Both players conduct second check to evaluate supply status for
        combat (410.22).

        5-3B: Available forces are organized (302.31). Prior to the initial
        combat round only, determine if small-scale combat Battle Force size
        restrictions (310.11) apply; if so, small- scale combat (310.0) must
        be used in Phase 5 - Step #3B and all following rounds in that Battle
        Hex.

        5-3C: Players secretly establish carrier and other escort groups
        (515.15). These cannot be changed until the Pursuit Phase. Exception
        (308.113). Certain tug and transport missions may be assigned; see
        (509.1).

        5-3D: Players secretly select flagships (302.32) based on eligible
        Command Ratings (302.33). Use Command Points (first round only) to
        increase command limits (308.9).

        5-3E: Players secretly select units to be in Battle Force, including
        drone bombardment ships and escorts. Designate units that are “in the
        web” (512.31). See (302.36) regarding minimum Battle Force
        requirements.

        5-3F: This sub-step is not used in Basic F&E.

        5-3G: Battle force deployments are announced; designate unit receiving
        Formation Bonus (308.7) and Scout Bonus (308.53). Designate uncrippled
        ships to tow crippled units from web (512.32). Reveal use of Command
        Points (308.92) and Drone Bombardment (309.0); all of this is done
        simultaneously.

        5-3H: This sub-step is not used in Basic F&E.

        5-3J: If applicable, resolve combat using small-scale combat (310.0),
        then skip to Phase 5 - Step 6.

        PHASE 5 - STEP 3X: BATTLE INTENSITY (302.3X)

        5-3X1: If fighting retreat, then set Battle Intensity Ratings of zero
        for the retreating force and 10 for the blocking force (302.772).

        5-3X2: Each player selects a Battle Intensity Rating (304.1). Attacker
        may raise intensity after a fourth combat round involving non-ship
        units (304.5). See exceptions for fighting retreats (302.771) and
        non-mobile defenses (304.4).

        5-3X3: Offensive use of cloaks (306.2) is resolved. 5-3X4: Determine
        Variable Battle Intensity (304.3).

        PHASE 5 - STEP 4: DAMAGE DETERMINATION (302.4)

        5-4A1: This sub-step is not used in Basic F&E.

        5-4A2: Calculate EW shift [(308.51)].

        5-4A3-A7: These sub-steps are not used in Basic F&E. 5-4B: This
        sub-step is not used in Basic F&E.

        5-4C1: Calculation of Combat Potential.

        5-4C2: Announce use of captured ships to gain +1 die roll modifier
        (305.21).

        5-4C3: Players roll die to determine Damage Coefficient (302.42).  See
        (308.6) for combining die-roll modifiers.

        5-4C4: Determine Combat Damage Result based on BIR, die roll, and any
        shifts, e.g., (308.51).

        5-4D: Remove captured ships used to gain benefit (305.21). 5-4E:
        Adjust damage result for minus points only (308.2).

        5-4F: This sub-step is not used in Basic F&E.

        5-4G: Determine if the attacker did sufficient damage in an Ap- proach
        Battle to proceed to base (308.3) in the next round.

        PHASE 5 - STEP 5: DIRECTED DAMAGE (302.5)

        5-5A: This step is not used in Basic F&E.

        5-5B: Phasing Player may select one Non-Phasing Player unit for
        Directed Damage and resolve this; exceptions: Pur- suit (307.4).

        5-5C: Non-Phasing Player may select one Phasing Player unit for
        Directed Damage and resolve this; exceptions: Pur- suit (307.4).

        5-5D: Adjust damage result for plus points (308.2). 5-5E: This
        sub-step is not used in Basic F&E.

        PHASE 5 - STEP 6: DAMAGE ALLOCATION (302.6)

        5-6A: Non-Phasing Player resolves general (non-directed) dam- age
        (302.6) to his units. See also (302.617).

        5-6B: Phasing Player resolves general (non-directed) damage (302.6) to
        his units. See also (302.617).

        5-6C: This sub-step is not used in Basic F&E.

        5-6D: Both players resolve shock for maulers (308.42).

        5-6E: Conduct ship transfers of fighters (501.6) and PFs
        (502.45). 5-6F: Surviving towing ships pull crippled units from web
        (512.32).

        PHASE 5 - STEP 6X: TROOP ACTION INTERPHASE (302.6X)

        5-6X1: Resolve Captured Ships (305.1); die roll modified by pur- suit
        (305.12). Maulers may make separate capture at- tempt in lieu of their
        directed damage use (308.47).

        5-6X2-3-4: These sub-steps are not used in Basic F&E.

        5-6X5: Transfer or lose fighters and PFs as a result of a destroyed
        defense battalion.

        5-6X6-8: These sub-steps are not used in Basic F&E. 5-6X9: Players may
        scuttle units they control (301.911).

        PHASE 5 - STEP 7: RETREAT (302.7)

        5-7A1: Defending Player first option to announce retreat (302.71).

        5-7A2: Attacking Player option to announce retreat (302.71).

        5-7A3: Defending Player last option to announce retreat (302.71).

        5-7A4: If neither player retreats return to Phase 5 - Step #2; if both
        retreat, then the Defending Player conducts his retreat first
        (302.712).

        5-7A5: If the Retreating Player leaves behind a base unit then return
        to Phase 5 - Step #2 until base is destroyed or if the other player
        chooses to retreat. Base units limit pur- suit options (302.741).

        5-7B: If one player chooses to pursue then proceed to Phase 5 - Step #8.

        5-7C: Analyze available retreat paths for ships (307.73).

        5-7D: Conduct ship retreat in accordance with the rules (302.72).

        Transports may be assigned to tow FRDs (509.1-F).

        PHASE 5 - STEP 8: PURSUIT (302.8) and (307.0)

        5-8A: Select pursuit units for attack against retreating ships and
        slow units (302.742); roll die to determine pursuit suc- cess; adjust
        die roll for declined approach battle (302.23).

        5-8B: Set up retreating Battle Force (307.3).

        5-8C: Conduct pursuit battle (repeating Phase 5, Step 3 through
        Step 6).

        5-8D: Conduct Slow Unit pursuit battle (repeating Phase 5, Step
        3 through Step 6).

        5-8E: Once all pursuit is resolved for the Battle Hex (302.8), then
        retreat is conducted immediately (302.72), (302.72C).

        5-8F: Analyze available retreat paths for units (307.73).

        5-8G: Conduct unit retreat in accordance with the rules (302.72).

        At the end of the Combat Phase, players may, but are not really required
        to, disband carrier groups and other groups.
    */
}

private static function retrograde_movement (team : String)
{
    yield WaitForSeconds(0.001); // TODO
    /*  PHASE 6: RETROGRADE MOVEMENT

        6A: Evaluate supply status for retrograde (410.24).

        6B: This step is not used in Basic F&E.

        6C: Dissolve carrier and other groups as desired.

        6C1: Phasing Player can retrograde some, all or none of his ships
        within the limitations (206.0).

        6C2: This sub-step is not used in F&E 2010.

        6C3: This sub-step is not used in F&E 2010.

        6D: This step was eliminated in F&E 2010.

        6E: All players may replace fighters (501.5) and PFs (502.43) if in
        supply.

        6F: Crippled tugs may send pods back to fleet pod pool (509.43). 6G:
        This step is not used in Basic F&E.
    */
}

private static function field_repairs (team : String)
{
    yield WaitForSeconds(0.001); // TODO
    /*  PHASE 7: FIELD REPAIRS (422.0)

        7A: Evaluate supply status for field repairs (410.23).

        7B: Deploy repair ships [and a repair tug from the capital (422.8)] to
        any SMN or FRD (422.3). Transports may be assigned mission E (509.1).

        7C: Repair units may repair only ships of the same empire (420.434)
        and (422.22).

        7D: Remove repair ships from the board (422.22); return repair tugs to
        the capital (422.8)
    */
}

private static function strategic_movement (team : String)
{
    yield WaitForSeconds(0.001); // TODO
    /*  PHASE 8: STRATEGIC MOVEMENT

        8A: Move eligible units via Strategic Movement (204.2). Some tugs and
        transports may be assigned new missions as per (509.1).

        8B: Tugs in expeditionary fleets may exchange pods (411.75). 8C:
        Phasing Player announces and begins deployment of mobile bases
        (510.22) and PDUs (or PGBs‡) (508.32).
    */
}

private static function establish_reserves (team : String)
{
    yield WaitForSeconds(0.001); // TODO
    /*  PHASE 9: ESTABLISH RESERVES

        9A: Phasing Player establishes reserve units (507.1). Some tugs and
        transports may be assigned new missions as per (509.1).

        9B: This sub-step is not used in Basic F&E.
    */
}

private static function assign_transports (team : String)
{
    yield WaitForSeconds(0.001); // TODO
    /*  PHASE 10: FINAL ACTIVITY

        10C: All players assigns transport missions as appropriate under
        sub-sections of (509.1).
    */
}

private static function scuttle (team : String)
{
    yield WaitForSeconds(0.001); // TODO
    /*  PHASE 10: FINAL ACTIVITY

        10D: All players may scuttle any unit they control (301.911).
    */
}

private static function team_turn (team : String)
{
    // PHASE 1: ECONOMICS
    yield RadicalRoutineExtensions.StartExtendedCoroutine(this_, economics(team));

    // PHASE 2: ECONOMICS
    yield RadicalRoutineExtensions.StartExtendedCoroutine(this_, repair(team));
    yield RadicalRoutineExtensions.StartExtendedCoroutine(this_, production(team));

    // PHASE 3: OPERATIONS
    yield RadicalRoutineExtensions.StartExtendedCoroutine(this_, operational_movement(team));

    // PHASE 4: RESERVE MOVEMENT
    yield RadicalRoutineExtensions.StartExtendedCoroutine(this_, reserve_movement(team));

    // PHASE 5: COMBAT
    yield RadicalRoutineExtensions.StartExtendedCoroutine(this_, combat(team));

    // PHASE 6: RETROGRADE MOVEMENT
    yield RadicalRoutineExtensions.StartExtendedCoroutine(this_, retrograde_movement(team));

    // PHASE 7: FIELD REPAIRS
    yield RadicalRoutineExtensions.StartExtendedCoroutine(this_, field_repairs(team));

    // PHASE 8: STRATEGIC MOVEMENT
    yield RadicalRoutineExtensions.StartExtendedCoroutine(this_, strategic_movement(team));

    // PHASE 9: ESTABLISH RESERVES
    yield RadicalRoutineExtensions.StartExtendedCoroutine(this_, establish_reserves(team));

    // PHASE 10: FINAL ACTIVITY
    yield RadicalRoutineExtensions.StartExtendedCoroutine(this_, assign_transports(team));
    yield RadicalRoutineExtensions.StartExtendedCoroutine(this_, scuttle(team));

    /*  PHASE 11: ORION PIRATE OPERATIONS
        The Orion Pirates conduct their turn as follows.

        11A: Recover all ships deployed for piracy and still on the map and
        return them to the Orion Force Pool.

        11B: All leased ships return to Orion control.

        11C1: Repair any crippled ships.

        11C2: Orions purchase new ships by (504.31).

        11D: Empires offer bribes to Orions, see (504.33).

        11E: The Orion player (or committee) decides what bribes to accept.

        11F: The Orions offer ships for lease auction, see (504.33). 11G:
        Empires bid on the Orion ships, and if successful, deploy them with
        their active fleets.

        11H: Orions deploy their ships available for Piracy using (504.34).

        The various empires deal with the ships during their turns as they
        would with any enemy.
    */
}

private static function game_main ()
{
    yield RadicalRoutineExtensions.StartExtendedCoroutine(this_, initial_setup());

    var scenario = this_.game_data_.scenario();
    var turn = scenario.start_turn;
    for (turn_ in scenario.turns) {
        for (team in scenario.team_turn_order) {
            yield RadicalRoutineExtensions.StartExtendedCoroutine(this_, team_turn(team));
        }
    }

    // TODO: Evaluate victory conditions for this scenario.

    // TODO: Transition into the next scenario, if appropriate.

    Debug.Log('Game Over');
}

function new_game (scenario_json : SimpleJSON.JSONNode, config : Dictionary.<String, boolean>)
{
    game_data_.load_data(scenario_json);

    player_nations_ = config;
    game_guid_ = System.Guid.NewGuid().ToString();

    while (!game_data_.map())
        yield WaitForSeconds(0.01);

    determine_supply(); // TODO Remove; for testing only.

    var scenario = game_data_.scenario();
    turn_ = scenario.start_turn;

    RadicalRoutineExtensions.StartExtendedCoroutine(gameObject, game_main());
}

function Awake ()
{// Debug.Log("test(3)=" + model_plugin.test(3)); // TODO
  var floats = new float[3];
  floats[0] = 1.0;
  floats[1] = 2.0;
  floats[2] = 3.0;
//  Debug.Log("test_2()=" + model_plugin.test_2(floats)); // TODO
  var hexes = new ga_hex_t[3];
  hexes[0].a = 1;
  hexes[1].a = 2;
  hexes[2].a = 3;
  hexes[0].b = 1.5;
  hexes[1].b = 2.5;
  hexes[2].b = 3.5;
//  Debug.Log("test_3()=" + model_plugin.test_3(hexes)); // TODO
    this_ = this;
}

function Update ()
{
    if (save_pending) {
        save();
        save_pending = false;
    }
}
