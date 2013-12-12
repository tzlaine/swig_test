#pragma strict

import SimpleJSON;
import System.IO;
import System.Collections.Generic;
import System.Globalization.NumberStyles;

private class capitol_hex
{
    var hex_id : String;
    var hex_x : int;
    var hex_y : int;
    var name : String;
    var planets : List.<String>;
};

private class nation_data
{
    var id : int;
    var name : String;
    var short_name : String;
    var abbr : String;
    var primary_color : Color;
    var secondary_color : Color;
    var dim_primary_color : Color;
    var dim_secondary_color : Color;
    var capitol : Dictionary.<String, capitol_hex>;
};

private class hex
{
    var x : int;
    var y : int;
    var owner : String; // abbreviated
    var owner_id : int;
    var province : int;
    var feature : String; // TODO: Make this all contents?
};

class map_t
{
    var hexes : hex[,];
};

class hex_x_y
{
    var x : int;
    var y : int;
};

static function str_to_hex_x_y (hex_str : String) : hex_x_y
{
    var retval = new hex_x_y();
    if (hex_str[0] == '0')
        hex_str = hex_str.Substring(1, 3);
    var hex_id : int = int.Parse(hex_str);
    retval.x = hex_id / 100 - 1;
    retval.y = hex_id % 100 - 1;
    return retval;
}


private var nations : Dictionary.<String, nation_data> = new Dictionary.<String, nation_data>();
private var map_ : map_t = new map_t();


function map () : map_t
{ return map_; }

function id (abbreviated_name : String)
{ return nations[abbreviated_name].id; }

function primary_color (abbreviated_name : String)
{ return nations[abbreviated_name].primary_color; }

function secondary_color (abbreviated_name : String)
{ return nations[abbreviated_name].primary_color; }

function str_to_color (str : String)
{
    var r : int = int.Parse(str.Substring(0, 2), AllowHexSpecifier);
    var g : int = int.Parse(str.Substring(2, 2), AllowHexSpecifier);
    var b : int = int.Parse(str.Substring(4, 2), AllowHexSpecifier);
    return Color(r / 255.0, g / 255.0, b / 255.0, 1);
}

function add_hex (m : map_t, hex_str : String, owner : String, province : int, feature : String)
{
    var hex_coord : hex_x_y = str_to_hex_x_y(hex_str);
    m.hexes[hex_coord.x, hex_coord.y] = new hex();
    var h = m.hexes[hex_coord.x, hex_coord.y];
    h.x = hex_coord.x;
    h.y = hex_coord.y;
    h.owner = owner;
    h.owner_id = id(owner);
    h.province = province;
    h.feature = feature;
}

function Awake ()
{
    var json : SimpleJSON.JSONNode;

    json = JSON.Parse(System.IO.File.ReadAllText('../nations.json'));

    var latest_id : int = 0;
    for (var nation : System.Collections.Generic.KeyValuePair.<String, JSONNode> in
         json) {
        nations[nation.Key] = new nation_data();
        nations[nation.Key].id = latest_id++;
        nations[nation.Key].primary_color = str_to_color(nation.Value['primary color']);
        nations[nation.Key].secondary_color = str_to_color(nation.Value['secondary color']);
        // TODO
    }

    json = JSON.Parse(System.IO.File.ReadAllText('../map.json'));

    var map_width = json['width'].AsInt;
    var map_height = json['height'].AsInt;

    map_.hexes = new hex[map_width, map_height];

    var zones = json['zones'];

    for (var zone : System.Collections.Generic.KeyValuePair.<String, JSONNode> in
         zones) {
        var i : int = 0;
        if (zone.Key == 'NZ') {
            var planets : Dictionary.<String, String> = new Dictionary.<String, String>();
            for (var hex : System.Collections.Generic.KeyValuePair.<String, JSONNode> in
                 zone.Value['planets']) {
                planets[hex.Key] = hex.Value.ToString();
            }
            for (i = 0; i < zone.Value['hexes'].Count; ++i) {
                var hex_str = zone.Value['hexes'][i];
                var feature = planets.ContainsKey(hex_str) ? planets[hex_str] : 'none';
                add_hex(map_, hex_str, "NZ", -1, feature);
            }
        } else {
            for (i = 0; i < zone.Value['provinces'].Count; ++i) {
                for (var hex : System.Collections.Generic.KeyValuePair.<String, JSONNode> in
                     zone.Value['provinces'][i]) {
                    add_hex(map_, hex.Key, zone.Key, i, hex.Value.ToString());
                }
            }
        }
    }
}
