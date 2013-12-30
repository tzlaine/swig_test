#pragma strict

import SimpleJSON;
import System.IO;
import System.Collections.Generic;
import System.Globalization.NumberStyles;
import System.Text.RegularExpressions;

var scenario_name : String = '../scenarios/the_wind.json';

private class capitol_hex
{
    function capitol_hex ()
    {
        hc = new hex_coord();
        planets = new List.<String>();
    }

    var hex_id : String;
    var hc : hex_coord;
    var name : String;
    var planets : List.<String>;
};

private class units_t
{
    function units_t ()
    {
        units = new Dictionary.<String, int>();
    }

    var units : Dictionary.<String, int>;
}

private class starting_fleet
{
    function starting_fleet ()
    {
        units = new units_t();
        reserve = false;
        pwc = new Dictionary.<int, units_t>();
    }

    var area : hex_coord[];
    var units : units_t;
    var reserve : boolean;
    var pwc : Dictionary.<int, units_t>; // key is turn id
};

private class mothball_reserve_t
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

private class production_turn_t
{
    function production_turn_t ()
    {
        units = new units_t();
    }

    var turn : int;
    var units : units_t;
}

private class nation_data
{
    function nation_data ()
    {
        capitol = new Dictionary.<String, capitol_hex>();
        starting_forces = new Dictionary.<String, starting_fleet>();
        mothball_reserve = new mothball_reserve_t();
    }

    var id : int;
    var name : String;
    var short_name : String;
    var abbr : String;
    var capitol : Dictionary.<String, capitol_hex>;
    var capitol_star_points : int;
    var starting_forces : Dictionary.<String, starting_fleet>;
    var mothball_reserve : mothball_reserve_t;
    var production : production_turn_t[];
};

private class hex
{
    function hex ()
    {
        hc = new hex_coord();
    }

    var hc : hex_coord;
    var owner : String; // abbreviated
    var owner_id : int;
    var province : int;
    var feature : String; // TODO: Make this all contents?
};

class offmap_area_t
{
    var owner : String;
    var owner_id : int;
    var name : String;
    var hexes : hex_coord[];
    var position : int; // 0: right, 1: top, 2: left // TODO: Add support for 3: bottom?
    var features : String[];
};

class map_t
{
    var hexes : hex[,];
    var offmap_areas : Dictionary.<String, offmap_area_t>;
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
    }
    var start_turn : int;
    var setup_order : String[];
    var nations : Dictionary.<String, scenario_nation_t>;
    var turns : scenario_turn_t[];
};

static function str_to_hex_coord (hex_str : String) : hex_coord
{
    var retval = new hex_coord();
    if (hex_str == 'offmap')
        return retval;
    if (hex_str[0] == '0')
        hex_str = hex_str.Substring(1, 3);
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

static function get_hex (hc : hex_coord, m : map_t) : hex
{
    var retval : hex = null;
    if (0 <= hc.x && hc.x < m.hexes.GetLength(0) &&
        0 <= hc.y && hc.y < m.hexes.GetLength(1)) {
        retval = m.hexes[hc.x, hc.y];
    }
    return retval;
}

static function hex_above (h : hex, m : map_t) : hex
{ return get_hex(hex_coord_above(h.hc), m); }

static function hex_below (h : hex, m : map_t) : hex
{ return get_hex(hex_coord_below(h.hc), m); }

static function hex_above_left (h : hex, m : map_t) : hex
{ return get_hex(hex_coord_above_left(h.hc), m); }

static function hex_above_right (h : hex, m : map_t) : hex
{ return get_hex(hex_coord_above_right(h.hc), m); }

static function hex_below_left (h : hex, m : map_t) : hex
{ return get_hex(hex_coord_below_left(h.hc), m); }

static function hex_below_right (h : hex, m : map_t) : hex
{ return get_hex(hex_coord_below_right(h.hc), m); }


private var nations : Dictionary.<String, nation_data> = new Dictionary.<String, nation_data>();
private var map_ : map_t = null;


function map () : map_t
{ return map_; }

function id (abbreviated_name : String) : int
{ return nations[abbreviated_name].id; }

function full_name (abbreviated_name : String) : String
{ return nations[abbreviated_name].name; }

function short_name (abbreviated_name : String) : String
{ return nations[abbreviated_name].short_name; }

function capitol_star_points (abbreviated_name : String)
{ return nations[abbreviated_name].capitol_star_points; }


private var scenario_ : scenario_t = null;

function scenario () : scenario_t
{ return scenario_; }


function add_hex (m : map_t, hex_str : String, owner : String, province : int, feature : String)
{
    var hc : hex_coord = str_to_hex_coord(hex_str);
    m.hexes[hc.x, hc.y] = new hex();
    var h = m.hexes[hc.x, hc.y];
    h.hc = hc;
    h.owner = owner;
    h.owner_id = id(owner);
    h.province = province;
    h.feature = feature;
}

private static function strip_quotes (str : String) : String
{ return str[0] == "'" || str[0] == '"' ? str.Substring(1, str.Length - 2) : str; }

private function populate_nations (json : SimpleJSON.JSONNode)
{
    var latest_id : int = 0;
    for (var nation : System.Collections.Generic.KeyValuePair.<String, JSONNode> in
         json) {
        nations[nation.Key] = new nation_data();
        nations[nation.Key].id = latest_id++;
        nations[nation.Key].capitol_star_points = nation.Value['capitol star points'].AsInt;
        nations[nation.Key].name = nation.Value['name'];
        nations[nation.Key].short_name = nation.Value['short name'];
        nations[nation.Key].abbr = nation.Key;
        // TODO
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
                retval.units.Add(match.Groups[2].Value, 0);
            retval.units[match.Groups[2].Value] += n;
        }
    }
    return retval;
}

static function parse_turn (json : SimpleJSON.JSONNode) : int
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
            var i = 0;
            for (i = 0; i < fleet.Value['hexes'].Count; ++i) {
                new_fleet.area[i] = str_to_hex_coord(fleet.Value['hexes'][i]);
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
            for (var j = 0; j < m.hexes.GetLength(1); ++j) {
                for (var i = 0; i < m.hexes.GetLength(0); ++i) {
                    if (m.hexes[i, j].owner == nation_name_)
                        hexes_.Push(m.hexes[i, j].hc);
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

private function make_map (json : SimpleJSON.JSONNode) : map_t
{
    var map_width = json['width'].AsInt;
    var map_height = json['height'].AsInt;

    var m : map_t = new map_t();
    m.hexes = new hex[map_width, map_height];
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
                if (offmap_area['features'].Count) {
                    oa.features = new String[offmap_area['features'].Count];
                    for (i = 0; i < offmap_area['features'].Count; ++i) {
                        oa.features[i] = strip_quotes(offmap_area['features'][i]);
                    }
                }
                oa.hexes = new hex_coord[offmap_area['adjacent hexes'].Count];
                for (i = 0; i < offmap_area['adjacent hexes'].Count; ++i) {
                    oa.hexes[i] =
                        str_to_hex_coord(strip_quotes(offmap_area['adjacent hexes'][i]));
                    if (0 < i) {
                        var position = 0;
                        if (oa.hexes[0].x == oa.hexes[i].x) {
                            position = oa.hexes[0].x ? 0 : 2; // right : left
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
    var json : SimpleJSON.JSONNode =
        JSON.Parse(System.IO.File.ReadAllText('../nations.json'));
    populate_nations(json);
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

    map_ = m;
}
