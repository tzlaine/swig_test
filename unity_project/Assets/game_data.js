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

private var nations : Dictionary.<String, nation_data> = new Dictionary.<String, nation_data>();


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

function Awake ()
{
    var json = JSON.Parse(System.IO.File.ReadAllText('../nations.json'));

    var latest_id : int = 0;
    for (var nation : System.Collections.Generic.KeyValuePair.<String, JSONNode> in
         json) {
        nations[nation.Key] = new nation_data();
        nations[nation.Key].id = latest_id++;
        nations[nation.Key].primary_color = str_to_color(nation.Value['primary color']);
        nations[nation.Key].secondary_color = str_to_color(nation.Value['secondary color']);
    }
}
