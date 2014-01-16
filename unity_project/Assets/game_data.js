#pragma strict

import SimpleJSON;
import System.IO;
import System.Collections.Generic;
import System.Globalization.NumberStyles;
import System.Text.RegularExpressions;
import pair;
import unit_costs_t;
import towable_stats_t;

@SerializeThis
private var nations : Dictionary.<String, nation_t> = new Dictionary.<String, nation_t>();

@SerializeThis
var nations_by_id : Dictionary.<int, String> = new Dictionary.<int, String>();

@SerializeThis
private var map_ : map_t = null;

@SerializeThis
private var scenario_ : scenario_t = null;

@SerializeThis
private var unit_defs : Dictionary.<String, Dictionary.<String, unit_def_t> > =
    new Dictionary.<String, Dictionary.<String, unit_def_t> >();

private var counters : Dictionary.<String, Dictionary.<String, counter_t> > =
    new Dictionary.<String, Dictionary.<String, counter_t> >();


class capital_hex_t
{
    function capital_hex_t ()
    {
        hc = new hex_coord();
        features = new List.<String>();
    }

    var hc : hex_coord;
    var name : String;
    var features : List.<String>;
};

class unit_stats_t
{
    var attack : int;
    var defense : int;
    var fighters : float;
    var heavy_fighter_bonus : float;
    var drones : int;
    var pfs : int;
    var scout : boolean;
    var mauler : boolean;
    var escort : int; // 0: non-escort, 1: light, 2: heavy
    var tug_missions : String[];
};

class unit_def_t
{
    function unit_t ()
    {
        conversions_from = new Dictionary.<String, unit_costs_t>();
        substitutions_for = new Dictionary.<String, unit_costs_t>();
    }

    var name : String;
    var command : int;
    var uncrippled : unit_stats_t;
    var crippled : unit_stats_t;
    var date_available : int;
    var construction : unit_costs_t;
    var conversions_from : Dictionary.<String, unit_costs_t>;
    var substitutions_for : Dictionary.<String, unit_costs_t>;
    var move : int;
    var carrier : int; // 0: non-carrier, light: 1, medium: 2, heavy: 3, single-ship: 4
    var spaceworthy : boolean;
    var towable : towable_stats_t;
    var salvage : float;
    var max_in_service : int;
};

class units_t
{
    function units_t ()
    {
        clear();
    }

    function clear ()
    { units = new Dictionary.<String, pair.<int, int> >(); }

    function size () : int
    {
        var retval = 0;
        for (u in units) {
            retval += u.Value.first;
            retval += u.Value.second;
        }
        return retval;
    }

    function insert (unit : String, crippled : boolean)
    {
        if (!units.ContainsKey(unit))
            units.Add(unit, pair.<int, int>(0, 0));
        var p = units[unit];
        if (crippled)
            p.second += 1;
        else
            p.first += 1;
        units[unit] = p;
    }

    function erase (unit : String, crippled : boolean)
    {
        var p = units[unit];
        if (crippled)
            p.second -= 1;
        else
            p.first -= 1;
        units[unit] = p;
        if (units[unit].first == 0 && units[unit].second == 0)
            units.Remove(unit);
    }

    var units : Dictionary.<String, pair.<int, int> >; // unit name -> uncrippled, crippled
}

class starting_fleet
{
    function starting_fleet ()
    {
        units = new units_t();
        reserve = false;
        pwc = new Dictionary.<int, units_t>();
    }

    var area : hex_coord[];
    var area_unit_limits : int[]; // 0 indicates no limit
    var units : units_t;
    var reserve : boolean;
    var pwc : Dictionary.<int, units_t>; // key is turn id
};

class mothball_reserve_t
{
    function mothball_reserve_t ()
    {
        units = new units_t();
        war_release = new units_t();
        limited_war_release = new units_t();
    }

    var units : units_t;
    var war_release : units_t;
    var limited_war_release : units_t;
};

class production_turn_t
{
    function production_turn_t ()
    {
        units = new units_t();
    }

    var turn : int;
    var units : units_t;
}

class nation_t
{
    function nation_t ()
    {
        capital = new Dictionary.<String, capital_hex_t>();
        starting_forces = new Dictionary.<String, starting_fleet>();
        mothball_reserve = new mothball_reserve_t();
    }

    var id : int;
    var name : String;
    var short_name : String;
    var abbr : String;
    var capital : Dictionary.<String, capital_hex_t>;
    var capital_star_points : int;
    var starting_forces : Dictionary.<String, starting_fleet>;
    var mothball_reserve : mothball_reserve_t;
    var production : production_turn_t[];
};

class hex_zone_t
{
    var name : String;
    var system : boolean;
    var features : String[]; // TODO
};

class hex_t
{
    function hex_t ()
    {
        hc = new hex_coord();
        units = new Dictionary.<String, units_t>();
    }

    function insert (nation : String, unit : String, crippled : boolean)
    {
        if (!units.ContainsKey(nation))
            units.Add(nation, new units_t());
        units[nation].insert(unit, crippled);
    }

    var hc : hex_coord;
    var owner : String; // abbreviated
    var owner_id : int;
    var province : int;
    var feature : String; // TODO: Replace this with zones below.
    var zones : hex_zone_t[]; // TODO: Replace feature with this.
    var units : Dictionary.<String, units_t>; // owner -> units
    var highlight : boolean;
};

class offmap_area_t
{
    function offmap_area_t ()
    {
        hex = new hex_t();
    }

    var owner : String;
    var owner_id : int;
    var name : String;
    var adjacent_hexes : hex_coord[];
    var position : int; // 0: right, 1: top, 2: left // TODO: Add support for 3: bottom?
    var features : String[]; // TODO: Replace this with hex.zones.

    // The container for all units, etc.  The .hc member is the 'hex' within
    // the offmap area into which to place counters.
    var hex : hex_t;
};

class map_t
{
    function map_t () {}

    function map_t (nations_by_id : Dictionary.<int, String>)
    {
        nations_by_id_ = nations_by_id;
    }

    function set_hex (hc : hex_coord, h : hex_t)
    { hexes[hc.x + hc.y * width] = h; }

    public function hex (hc : hex_coord) : hex_t
    {
        if (hc.x == hex_coord().x) {
            var id = hc.y;
            return offmap_areas[nations_by_id_[id]].hex;
        }
        return hexes[hc.x + hc.y * width];
    }

    var width : int;
    var height : int;
    var hexes : hex_t[];
    var offmap_areas : Dictionary.<String, offmap_area_t>;

    var nations_by_id_ : Dictionary.<int, String>;
};

class hex_coord
{
    public function hex_coord ()
    {
        x = 1000;
        y = 1000;
    }

    public function hex_coord (x_ : int, y_ : int)
    {
        x = x_;
        y = y_;
    }

    public function Equals (rhs_ : Object) : boolean
    {
        if (rhs_ == null)
            return false;
        var rhs : hex_coord = rhs_ as hex_coord;
        if (rhs == null)
            return false;

        return x == rhs.x && y == rhs.y;
    }

    public function GetHashCode () : int
    { return x * 100 + y; }

    var x : int;
    var y : int;
};

class victory_condition_t
{
    // TODO
};

class victory_conditions_t
{
    function victory_conditions_t ()
    {
        conditions = Dictionary.<String, victory_condition_t>();
    }

    var conditions : Dictionary.<String, victory_condition_t>;
};

class event_condition_t
{
    function event_condition_t ()
    {}

    function event_condition_t (subject_ : String[])
    {
        subject = subject_;
        listener = null;
    }

    function object_destroyed (object : GameObject)
    {}
    function fleet_moved (hc : hex_coord)
    {}

    var subject : String[];
    var listener : conditional_event_t;
};

class occupy_area extends event_condition_t
{
    function occupy_area ()
    {}

    function occupy_area (subject_ : String[], hexes_ : hex_coord[])
    {
        super(subject_);
        hexes = hexes_;
    }

    function fleet_moved (hc : hex_coord)
    {
        if (/* TODO */ false && listener) {
            listener.trigger();
        }
    }

    var hexes : hex_coord[];
};

class destroy_object extends event_condition_t
{
    function destroy_object ()
    {}

    function destroy_object (subject_ : String[], type_: String, hexes_ : hex_coord[])
    {
        super(subject_);
        type = type_;
        hexes = hexes_;
    }

    function object_destroyed (object : GameObject)
    {
        if (/* TODO */ false && listener) {
            listener.trigger();
        }
    }

    var type : String;
    var hexes : hex_coord[];
};

class conditional_event_t
{
    function trigger ()
    {}
};

class release_condition_t extends conditional_event_t
{
    function trigger ()
    {
        // TODO
    }

    var fleet : String;
    var nation : String;
    var condition : event_condition_t;
};

class war_entry_condition_t extends conditional_event_t
{
    function trigger ()
    {
        // TODO
    }

    var nation : String;
    var limited_war : boolean;
    var condition : event_condition_t;
};

class scenario_nation_t
{
    var at_war_with : String[];
    var future_belligerents : String[];
    var economy : String;
    var exhaustion_turns : int;
    var accumulate_exhaustion_at_peace : boolean;
    var release_conditions : release_condition_t[];
    var war_entry_conditions : war_entry_condition_t[];
};

class nation_scenario_turn_t
{
    var nation : String;
    var declare_war_on : String[];
    var release_fleets : String[];
};

class scenario_turn_t
{
    var nations : nation_scenario_turn_t[];
};

class scenario_t
{
    function scenario_t ()
    {
        nations = new Dictionary.<String, scenario_nation_t>();
        teams = new Dictionary.<String, String[]>();
    }

    var name : String;
    var start_turn : int;
    var setup_order : String[];
    var nations : Dictionary.<String, scenario_nation_t>;
    var teams : Dictionary.<String, String[]>;
    var team_turn_order : String[];
    var turns : scenario_turn_t[];
};

class counter_side_t
{
    function counter_side_t () {}
    function counter_side_t (rhs : counter_side_t)
    {
        texture_filename = rhs.texture_filename;
        uv_min = rhs.uv_min;
        uv_max = rhs.uv_max;
        texture = rhs.texture;
    }
    var texture_filename : String;
    var uv_min : Vector2;
    var uv_max : Vector2;
    var texture : Texture2D;
};

class counter_t
{
    var uncrippled_side : counter_side_t;
    var crippled_side : counter_side_t;
};

function counter (nation : String, unit : String)
{ return counters[nation][unit]; }

static function crippled_side (uncrippled_side : counter_side_t) : counter_side_t
{
    var retval = new counter_side_t(uncrippled_side);

    if (!retval.texture_filename || retval.texture_filename == '')
        return retval;

    retval.texture_filename = retval.texture_filename.Replace('front', 'back');
    retval.uv_min.x = 1.0 - uncrippled_side.uv_max.x;
    retval.uv_max.x = 1.0 - uncrippled_side.uv_min.x;

    return retval;
}

static function str_to_hex_coord (hex_str : String) : hex_coord
{ return str_to_hex_coord(hex_str, hex_coord().y); }

private static function str_to_hex_coord (hex_str : String, nation_id : int) : hex_coord
{
    var retval = new hex_coord();
    if (hex_str == 'offmap') {
        retval.y = nation_id;
        return retval;
    }
    if (hex_str[0] == '0')
        hex_str = hex_str.Substring(1, 3);
    if (hex_str[0] == '0')
        hex_str = hex_str.Substring(1, 2);
    if (hex_str[0] == '0')
        hex_str = hex_str.Substring(1, 1);
    var hex_id : int = int.Parse(hex_str);
    retval.x = hex_id / 100 - 1;
    retval.y = hex_id % 100 - 1;
    return retval;
}

static function hex_coord_above (hc : hex_coord) : hex_coord
{ return hex_coord(hc.x, hc.y - 1); }

static function hex_coord_below (hc : hex_coord) : hex_coord
{ return hex_coord(hc.x, hc.y + 1); }

static function hex_coord_above_left (hc : hex_coord) : hex_coord
{ return hex_coord(hc.x - 1, hc.y + (hc.x % 2 ? 0 : -1)); }

static function hex_coord_below_left (hc : hex_coord) : hex_coord
{ return hex_coord(hc.x - 1, hc.y + (hc.x % 2 ? 1 : 0)); }

static function hex_coord_above_right (hc : hex_coord) : hex_coord
{ return hex_coord(hc.x + 1, hc.y + (hc.x % 2 ? 0 : -1)); }

static function hex_coord_below_right (hc : hex_coord) : hex_coord
{ return hex_coord(hc.x + 1, hc.y + (hc.x % 2 ? 1 : 0)); }

static function get_hex (hc : hex_coord, m : map_t) : hex_t
{
    var retval : hex_t = null;
    if (0 <= hc.x && hc.x < m.width && 0 <= hc.y && hc.y < m.height)
        retval = m.hex(hc);
    return retval;
}

static function hex_above (h : hex_t, m : map_t) : hex_t
{ return get_hex(hex_coord_above(h.hc), m); }

static function hex_below (h : hex_t, m : map_t) : hex_t
{ return get_hex(hex_coord_below(h.hc), m); }

static function hex_above_left (h : hex_t, m : map_t) : hex_t
{ return get_hex(hex_coord_above_left(h.hc), m); }

static function hex_above_right (h : hex_t, m : map_t) : hex_t
{ return get_hex(hex_coord_above_right(h.hc), m); }

static function hex_below_left (h : hex_t, m : map_t) : hex_t
{ return get_hex(hex_coord_below_left(h.hc), m); }

static function hex_below_right (h : hex_t, m : map_t) : hex_t
{ return get_hex(hex_coord_below_right(h.hc), m); }


function map () : map_t
{ return map_; }

function id (abbreviated_name : String) : int
{ return nations[abbreviated_name].id; }

function full_name (abbreviated_name : String) : String
{ return nations[abbreviated_name].name; }

function short_name (abbreviated_name : String) : String
{ return nations[abbreviated_name].short_name; }

function capital_star_points (abbreviated_name : String)
{ return nations[abbreviated_name].capital_star_points; }


function nation (abbreviated_name : String) : nation_t
{ return nations[abbreviated_name]; }

function scenario () : scenario_t
{ return scenario_; }


function add_hex (m : map_t, hex_str : String, owner : String, province : int, feature : String)
{
    var hc : hex_coord = str_to_hex_coord(hex_str);
    var h = new hex_t();
    h.hc = hc;
    h.owner = owner;
    h.owner_id = id(owner);
    h.province = province;
    h.feature = feature;
    m.set_hex(hc, h);
}

private static function strip_quotes (str : String) : String
{ return str[0] == "'" || str[0] == '"' ? str.Substring(1, str.Length - 2) : str; }

private function populate_nations (json : SimpleJSON.JSONNode)
{
    var latest_id : int = 0;
    for (var nation : System.Collections.Generic.KeyValuePair.<String, JSONNode> in
         json) {
        nations.Add(nation.Key, new nation_t());
        nations[nation.Key].id = latest_id++;
        nations[nation.Key].capital_star_points = nation.Value['capital star points'].AsInt;
        nations[nation.Key].name = nation.Value['name'];
        nations[nation.Key].short_name = nation.Value['short name'];
        nations[nation.Key].abbr = nation.Key;

        for (var capital : System.Collections.Generic.KeyValuePair.<String, JSONNode> in
             nation.Value['capital']) {
            nations[nation.Key].capital.Add(capital.Key, new capital_hex_t());
            var hex = nations[nation.Key].capital[capital.Key];
            for (var system : System.Collections.Generic.KeyValuePair.<String, JSONNode> in
                 capital.Value) {
                hex.hc = str_to_hex_coord(capital.Key);
                hex.name = system.Key;
                var features = parse_strings(system.Value);
                for (feature in features) {
                    hex.features.Add(feature);
                }
            }
        }

        nations_by_id.Add(nations[nation.Key].id, nations[nation.Key].abbr);
    }
}

private static var unit_regex = '(?:(\\d+)x)?(.+)';
private function parse_units (json : SimpleJSON.JSONNode) : units_t
{
    var retval = new units_t();
    for (var i = 0; i < json.Count; ++i) {
        var str = json[i];
        for (var match : Match in Regex.Matches(str, unit_regex)) {
            var number_str : String = match.Groups[1].Value;
            var n = number_str == '' ? 1 : int.Parse(number_str);
            if (!retval.units.ContainsKey(match.Groups[2].Value))
                retval.units.Add(match.Groups[2].Value, pair.<int, int>(0, 0));
            var p = retval.units[match.Groups[2].Value];
            p.first += n;
            retval.units[match.Groups[2].Value] = p;
        }
    }
    return retval;
}

private static function parse_turn (json : SimpleJSON.JSONNode) : int
{
    var retval : int = json['year'].AsInt * 10;
    var season : String = json['season'];
    retval += season == 'spring' ? 0 : 1;
    return retval;
}

private function populate_oob (json : SimpleJSON.JSONNode)
{
    for (var nation : System.Collections.Generic.KeyValuePair.<String, JSONNode> in
         json) {
        var forces = nations[nation.Key].starting_forces;
        for (var fleet : System.Collections.Generic.KeyValuePair.<String, JSONNode> in
             nation.Value['starting force']) {
            var new_fleet = new starting_fleet();
            new_fleet.area = new hex_coord[fleet.Value['hexes'].Count];
            new_fleet.area_unit_limits = new int[fleet.Value['hexes'].Count];
            var max_units : SimpleJSON.JSONNode = fleet.Value['max units in'];
            var i = 0;
            for (i = 0; i < fleet.Value['hexes'].Count; ++i) {
                var hex_str : String = fleet.Value['hexes'][i];
                new_fleet.area[i] = str_to_hex_coord(hex_str, nations[nation.Key].id);
                if (max_units != null && max_units[hex_str] != null)
                    new_fleet.area_unit_limits[i] = max_units[hex_str].AsInt;
            }
            new_fleet.units = parse_units(fleet.Value['units']);
            if (fleet.Value['reserve'] != null)
                new_fleet.reserve = true;
            for (i = 0; i < fleet.Value['PWC'].Count; ++i) {
                var pwc = fleet.Value['PWC'][i];
                var turn = parse_turn(pwc);
                var units = parse_units(pwc['units']);
                new_fleet.pwc.Add(turn, units);
            }
            forces.Add(fleet.Key, new_fleet);
        }

        nations[nation.Key].production =
            new production_turn_t[nation.Value['production'].Count];
        for (i = 0; i < nation.Value['production'].Count; ++i) {
            var prod = nation.Value['production'][i];
            nations[nation.Key].production[i] = new production_turn_t();            
            nations[nation.Key].production[i].turn = parse_turn(prod);
            nations[nation.Key].production[i].units = parse_units(prod['units']);
        }

        var mothball_reserve = nation.Value['mothball reserve'];
        nations[nation.Key].mothball_reserve.units =
            parse_units(mothball_reserve['units']);
        nations[nation.Key].mothball_reserve.war_release =
            parse_units(mothball_reserve['war release']);
        nations[nation.Key].mothball_reserve.limited_war_release =
            parse_units(mothball_reserve['limited war release']);
    }
}

private function parse_strings (json : SimpleJSON.JSONNode) : String[]
{
    if (!json)
        return null;
    var retval = new String[json.Count];
    for (var i = 0; i < json.Count; ++i) {
        retval[i] = json[i];
    }
    return retval;
}

private function parse_condition (
    json : SimpleJSON.JSONNode,
    future_belligerents : String[],
    m : map_t
) : event_condition_t
{
    var retval : event_condition_t = null;

    var subjects = future_belligerents;
    var subject_str : String = json[0];
    if (subject_str != 'future belligerents') {
        subjects = new String[1];
        subjects[0] = subject_str;
    }

    var hexes : hex_coord[] = null;

    var verb : String = json[1];
    if (verb == 'occupies') {
        if (json[2]['fleet area'] != null) {
            var fleet_name : String = json[2]['fleet area'];
            var nation_name : String = json[2]['nation'];
            hexes = nations[nation_name].starting_forces[fleet_name].area;
        } else if (json[2]['hex'] != null) {
            hexes = new hex_coord[1];
            hexes[0] = str_to_hex_coord(json[2]['hex']);
        } else if (json[2]['nation'] != null) {
            var hexes_ = new Array();
            var nation_name_ : String = json[2]['nation'];
            for (var j = 0; j < m.height; ++j) {
                for (var i = 0; i < m.width; ++i) {
                    var hc = hex_coord(i, j);
                    if (m.hex(hc).owner == nation_name_)
                        hexes_.Push(hc);
                }
            }
            hexes = new hex_coord[hexes_.length];
            for (var k = 0; k < hexes_.length; ++i) {
                hexes[i] = hexes_[i];
            }
        }
        retval = new occupy_area(subjects, hexes);
    } else if (verb == 'destroys') {
        for (var destroyee : System.Collections.Generic.KeyValuePair.<String, JSONNode> in
             json[2]) {
            var hex_strings = parse_strings(destroyee.Value);
            if (1 < hex_strings.Length || hex_strings[0] != 'any') {
                hexes = new hex_coord[hex_strings.Length];
                for (var l = 0; l < hexes.Length; ++l) {
                    hexes[l] = str_to_hex_coord(hex_strings[l]);
                }
            }
            retval = new destroy_object(subjects, destroyee.Key, hexes);
        }
    }
    return retval;
}

private function populate_scenario (json : SimpleJSON.JSONNode, m : map_t)
{
    var i = 0;

    scenario_ = new scenario_t();

    var scenario_name : String = json['name'];
    scenario_.name = scenario_name;

    for (var team : System.Collections.Generic.KeyValuePair.<String, JSONNode> in
         json['teams']) {
        scenario_.teams.Add(team.Key, parse_strings(team.Value));
    }

    scenario_.team_turn_order = parse_strings(json['team turn order']);

    scenario_.start_turn = parse_turn(json['start turn']);
    scenario_.setup_order = parse_strings(json['setup order']);

    for (var nation : System.Collections.Generic.KeyValuePair.<String, JSONNode> in
         json['nations']) {
        var scenario_nation = new scenario_nation_t();
        scenario_nation.at_war_with = parse_strings(nation.Value['at war with']);
        scenario_nation.future_belligerents = parse_strings(nation.Value['future belligerents']);
        scenario_nation.economy = nation.Value['economy'].ToString();
        scenario_nation.exhaustion_turns = nation.Value['exhaustion turns'].AsInt;
        var accumulate_exhaustion : String = nation.Value['accumulate exhaustion at peace'];
        scenario_nation.accumulate_exhaustion_at_peace = accumulate_exhaustion == 'true';

        if (nation.Value['release conditions'].Count) {
            i = 0;
            scenario_nation.release_conditions =
                new release_condition_t[nation.Value['release conditions'].Count];
            for (var release_condition : System.Collections.Generic.KeyValuePair.<String, JSONNode> in
                 nation.Value['release conditions']) {
                var new_release_condition = new release_condition_t();
                new_release_condition.fleet = release_condition.Key;
                new_release_condition.nation = nation.Key;
                new_release_condition.condition =
                    parse_condition(release_condition.Value,
                                    scenario_nation.future_belligerents,
                                    m);
                new_release_condition.condition.listener = new_release_condition;
                scenario_nation.release_conditions[i++] = new_release_condition;
            }
        }

        if (nation.Value['war entry conditions'].Count) {
            i = 0;
            scenario_nation.war_entry_conditions =
                new war_entry_condition_t[nation.Value['war entry conditions'].Count];
            for (var war_entry_condition : System.Collections.Generic.KeyValuePair.<String, JSONNode> in
                 nation.Value['war entry conditions']) {
                var new_war_entry_condition = new war_entry_condition_t();
                new_war_entry_condition.nation = nation.Key;
                new_war_entry_condition.limited_war = war_entry_condition.Key == 'limited war';
                new_war_entry_condition.condition =
                    parse_condition(war_entry_condition.Value,
                                    scenario_nation.future_belligerents,
                                    m);
                new_war_entry_condition.condition.listener = new_war_entry_condition;
                scenario_nation.war_entry_conditions[i++] = new_war_entry_condition;
            }
        }

        scenario_.nations.Add(nation.Key, scenario_nation);
    }

    scenario_.turns = new scenario_turn_t[json['turns'].Count];
    i = 0;
    for (var turn : System.Collections.Generic.KeyValuePair.<String, JSONNode> in
         json['turns']) {
        var scenario_turn = new scenario_turn_t();
        scenario_turn.nations = new nation_scenario_turn_t[turn.Value.Count];
        var j = 0;
        for (var nation_turn : System.Collections.Generic.KeyValuePair.<String, JSONNode> in
             turn.Value) {
            var scenario_nation_turn = new nation_scenario_turn_t();
            scenario_nation_turn.nation = nation_turn.Key;
            scenario_nation_turn.declare_war_on =
                parse_strings(nation_turn.Value['declare war']);
            scenario_nation_turn.release_fleets =
                parse_strings(nation_turn.Value['release fleets']);
            scenario_turn.nations[j++] = scenario_nation_turn;
        }
        scenario_.turns[i++] = scenario_turn;
    }

    // TODO: victory conditions
}

private function populate_counters (json : SimpleJSON.JSONNode)
{
    for (var n : System.Collections.Generic.KeyValuePair.<String, JSONNode> in
         json) {
        var nation_counters = new Dictionary.<String, counter_t>();
        for (var u : System.Collections.Generic.KeyValuePair.<String, JSONNode> in
             n.Value) {
            var counter_side = new counter_side_t();
            counter_side.texture_filename = u.Value['texture'];
            counter_side.uv_min = Vector2(
                u.Value['uv_min']['u'].AsFloat,
                u.Value['uv_min']['v'].AsFloat
            );
            counter_side.uv_max = Vector2(
                u.Value['uv_max']['u'].AsFloat,
                u.Value['uv_max']['v'].AsFloat
            );

            var counter = new counter_t();
            counter.uncrippled_side = counter_side;
            counter.crippled_side = crippled_side(counter_side);
            nation_counters.Add(u.Key, counter);
        }
        counters.Add(n.Key, nation_counters);
    }
}

private function parse_unit_stats (json : SimpleJSON.JSONNode)
{
    var retval : unit_stats_t = null;
    if (json != null) {
        retval = new unit_stats_t();
        retval.attack = json['att'].AsInt;
        retval.defense = json['def'].AsInt;
        if (json['fighters'])
            retval.fighters = json['fighters'].AsFloat;
        if (json['heavy fighter bonus'])
            retval.heavy_fighter_bonus = json['heavy fighter bonus'].AsFloat;
        if (json['drones'])
            retval.drones = json['drones'].AsInt;
        if (json['PFs'])
            retval.pfs = json['PFs'].AsInt;
        if (json['scout'])
            retval.scout = true;
        if (json['mauler'])
            retval.mauler = true;
        if (json['escort']) {
            var escort : String = json['escort'];
            if (escort == 'light')
                retval.escort = 1;
            else if (escort == 'heavy')
                retval.escort = 2;
        }
        retval.tug_missions = parse_strings(json['tug missions']);
    }
    return retval;
}

private function parse_unit_costs (json : SimpleJSON.JSONNode)
{
    var retval : unit_costs_t;
    if (json != null) {
        retval.cost = json['cost'].AsInt;
        if (json['fighter cost'])
            retval.fighter_cost = json['fighter cost'].AsInt;
    }
    return retval;
}

private function populate_units (json : SimpleJSON.JSONNode)
{
    for (var n : System.Collections.Generic.KeyValuePair.<String, JSONNode> in
         json) {
        var nation_unit_defs = new Dictionary.<String, unit_def_t>();
        for (var u : System.Collections.Generic.KeyValuePair.<String, JSONNode> in
             n.Value) {
            var unit_def = new unit_def_t();
            unit_def.name = u.Key;
            unit_def.command = u.Value['cmd'].AsInt;
            unit_def.uncrippled = parse_unit_stats(u.Value['uncrippled']);
            unit_def.crippled = parse_unit_stats(u.Value['crippled']);
            unit_def.date_available = parse_turn(u.Value['date available']);
            unit_def.construction = parse_unit_costs(u.Value['construction']);
            if (u.Value['conversions from'] != null) {
                if (!unit_def.conversions_from)
                    unit_def.conversions_from = new Dictionary.<String, unit_costs_t>();
                for (var conv : System.Collections.Generic.KeyValuePair.<String, JSONNode> in
                     u.Value['conversions from']) {
                    if (conv.Key != 'TODO')
                        unit_def.conversions_from.Add(conv.Key, parse_unit_costs(conv.Value));
                }
            }
            if (u.Value['substitutions for'] != null) {
                if (!unit_def.substitutions_for)
                    unit_def.substitutions_for = new Dictionary.<String, unit_costs_t>();
                for (var sub : System.Collections.Generic.KeyValuePair.<String, JSONNode> in
                     u.Value['substitutions for']) {
                    if (sub.Key != 'TODO')
                        unit_def.substitutions_for.Add(sub.Key, parse_unit_costs(sub.Value));
                }
            }
            unit_def.move = u.Value['move'].AsInt;
            if (u.Value['carrier']) {
                var carrier : String = u.Value['carrier'];
                if (carrier == 'light')
                    unit_def.carrier = 1;
                else if (carrier == 'medium')
                    unit_def.carrier = 2;
                else if (carrier == 'heavy')
                    unit_def.carrier = 3;
                else if (carrier == 'single-ship')
                    unit_def.carrier = 4;
            }
            if (u.Value['spaceworthy'])
                unit_def.spaceworthy = false;
            if (u.Value['towable']) {
                unit_def.towable.move_cost = u.Value['towable']['move cost'].AsInt;
                unit_def.towable.strategic_move_limit =
                    u.Value['towable']['strat move limit'].AsInt;
            }
            unit_def.salvage = u.Value['salvage'].AsFloat;
            unit_def.max_in_service = u.Value['max in service'].AsInt;
            nation_unit_defs.Add(u.Key, unit_def);
        }
        unit_defs.Add(n.Key, nation_unit_defs);
    }
}

private function make_map (json : SimpleJSON.JSONNode) : map_t
{
    var m : map_t = new map_t(nations_by_id);
    m.width = json['width'].AsInt;
    m.height = json['height'].AsInt;
    m.hexes = new hex_t[m.width * m.height];
    m.offmap_areas = new Dictionary.<String, offmap_area_t>();

    var zones = json['zones'];

    var province : int = 0;
    for (var zone : System.Collections.Generic.KeyValuePair.<String, JSONNode> in
         zones) {
        var i : int = 0;
        if (zone.Key == 'NZ') {
            var planets : Dictionary.<String, String> = new Dictionary.<String, String>();
            for (var hex : System.Collections.Generic.KeyValuePair.<String, JSONNode> in
                 zone.Value['planets']) {
                planets[strip_quotes(hex.Key)] = strip_quotes(hex.Value.ToString());
            }
            for (i = 0; i < zone.Value['hexes'].Count; ++i) {
                var hex_str = strip_quotes(zone.Value['hexes'][i]);
                var feature = planets.ContainsKey(hex_str) ? strip_quotes(planets[hex_str]) : 'none';
                add_hex(m, hex_str, "NZ", -1, feature);
            }
        } else {
            for (i = 0; i < zone.Value['provinces'].Count; ++i) {
                for (var hex : System.Collections.Generic.KeyValuePair.<String, JSONNode> in
                     zone.Value['provinces'][i]) {
                    add_hex(m, hex.Key, zone.Key, province, strip_quotes(hex.Value.ToString()));
                }
                ++province;
            }

            if (zone.Value['offmap area'] != null) {
                var offmap_area = zone.Value['offmap area'];
                var oa = new offmap_area_t();
                oa.owner = zone.Key;
                oa.owner_id = id(oa.owner);
                oa.name = strip_quotes(offmap_area['name']);
                oa.hex.hc = str_to_hex_coord(strip_quotes(offmap_area['counter hex']));
                if (offmap_area['features'].Count) {
                    oa.features = new String[offmap_area['features'].Count];
                    for (i = 0; i < offmap_area['features'].Count; ++i) {
                        oa.features[i] = strip_quotes(offmap_area['features'][i]);
                    }
                }
                oa.adjacent_hexes = new hex_coord[offmap_area['adjacent hexes'].Count];
                for (i = 0; i < offmap_area['adjacent hexes'].Count; ++i) {
                    oa.adjacent_hexes[i] =
                        str_to_hex_coord(strip_quotes(offmap_area['adjacent hexes'][i]));
                    if (0 < i) {
                        var position = 0;
                        if (oa.adjacent_hexes[0].x == oa.adjacent_hexes[i].x) {
                            position = oa.adjacent_hexes[0].x ? 0 : 2; // right : left
                        } else {
                            position = 1; // top
                        }
                        oa.position = position;
                    }
                }
                m.offmap_areas[zone.Key] = oa;
            }
        }
    }

    return m;
}

function Awake ()
{
    if (!JSONLevelSerializer.IsDeserializing) {
        var json : SimpleJSON.JSONNode =
            JSON.Parse(System.IO.File.ReadAllText('../nations.json'));
        populate_nations(json);
    }

    json = JSON.Parse(System.IO.File.ReadAllText('../unit_counters.json'));
    populate_counters(json);

    enabled = false;
}

function load_data (scenario : SimpleJSON.JSONNode)
{
    var json : SimpleJSON.JSONNode;

    var map_filename : String = scenario['map'];
    json = JSON.Parse(System.IO.File.ReadAllText(map_filename));
    var m : map_t = make_map(json);

    var oob_filename : String = scenario['order of battle'];
    json = JSON.Parse(System.IO.File.ReadAllText(oob_filename));
    populate_oob(json);

    populate_scenario(scenario, m);

    json = JSON.Parse(System.IO.File.ReadAllText('../units.json'));
    populate_units(json);

    map_ = m;
}
