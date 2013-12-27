#pragma strict

import SimpleJSON;
import System.IO;
import System.Collections.Generic;
import System.Globalization.NumberStyles;

private class capitol_hex
{
    var hex_id : String;
    var hc : hex_coord;
    var name : String;
    var planets : List.<String>;
};

private class nation_data
{
    var id : int;
    var name : String;
    var short_name : String;
    var abbr : String;
    var capitol : Dictionary.<String, capitol_hex>;
    var capitol_star_points : int;
};

private class hex
{
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
    var position : int; // 0: right, 1: top, 2: left
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

static function str_to_hex_coord (hex_str : String) : hex_coord
{
    var retval = new hex_coord();
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

function id (abbreviated_name : String)
{ return nations[abbreviated_name].id; }

function capitol_star_points (abbreviated_name : String)
{ return nations[abbreviated_name].capitol_star_points; }

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

function Awake ()
{
    var json : SimpleJSON.JSONNode;

    json = JSON.Parse(System.IO.File.ReadAllText('../nations.json'));

    var latest_id : int = 0;
    for (var nation : System.Collections.Generic.KeyValuePair.<String, JSONNode> in
         json) {
        nations[nation.Key] = new nation_data();
        nations[nation.Key].id = latest_id++;
        nations[nation.Key].capitol_star_points = nation.Value['capitol star points'].AsInt;
        // TODO
    }

    json = JSON.Parse(System.IO.File.ReadAllText('../map.json'));

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

    map_ = m;
}
