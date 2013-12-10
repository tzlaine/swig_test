#pragma strict

import SimpleJSON;
import System.IO;

var hex_surface : GameObject;

private var map_width : int = 0;
private var map_height : int = 0;
private var map_origin : Vector3 = Vector3(0, 0, 0);

private static var sin_60 : float = Mathf.Sin(Mathf.Deg2Rad * 60);

function place_tile (x : int, y : int, color : Color)
{
    var obj : GameObject = Instantiate(hex_surface);
    obj.transform.position =
        Vector3(x * 1.5, (map_height - 1 - y) * 2 * sin_60, 0) - map_origin;
    if (x % 2 == 1)
        obj.transform.position.y -= sin_60;
    obj.renderer.material.color = color;
}

function process_map_hex (hex_str : String, color : Color)
{
    if (hex_str[0] == '0')
        hex_str = hex_str.Substring(1, 3);
    var hex_id : int = int.Parse(hex_str);
    var hex_x = hex_id / 100 - 1;
    var hex_y = hex_id % 100 - 1;
    place_tile(hex_x, hex_y, color);
}

function Start ()
{
    // Get map data.
    var map_data = JSON.Parse(System.IO.File.ReadAllText('../map.json'));

    map_width = map_data['width'].AsInt;
    map_height = map_data['height'].AsInt;
    map_origin = Vector3((map_width - 1) * 1.5 / 2, (map_height - 1) * 2 * sin_60 / 2, 0);

    var zones = map_data['zones'];

    for (var zone : System.Collections.Generic.KeyValuePair.<String, JSONNode> in
         zones) {
        var color : Color = Color.gray;
        if (zone.Key == 'Federation') {
            color = Color.blue; // TODO
        } else if (zone.Key == 'Gorn') {
            color = Color.white; // TODO
        } else if (zone.Key == 'Interstellar Concordium') {
            color = Color.yellow; // TODO
        } else if (zone.Key == 'Hydran') {
            color = Color.green; // TODO
        } else if (zone.Key == 'Klingon') {
            color = Color.black; // TODO
        } else if (zone.Key == 'Kzinti') {
            color = Color.white; // TODO
        } else if (zone.Key == 'Lyran') {
            color = Color.yellow; // TODO
        } else if (zone.Key == 'Lyran Democratic Republic') {
            color = Color.yellow; // TODO
        } else if (zone.Key == 'Romulan') {
            color = Color.red; // TODO
        } else if (zone.Key == 'Tholian') {
            color = Color.red; // TODO
        }

        var i : int = 0;
        if (zone.Key == 'Neutral Zone') {
            for (i = 0; i < zone.Value['hexes'].Count; ++i) {
                process_map_hex(zone.Value['hexes'][i], color);
            }
        } else {
            for (i = 0; i < zone.Value['provinces'].Count; ++i) {
                for (var hex : System.Collections.Generic.KeyValuePair.<String, JSONNode> in
                     zone.Value['provinces'][i]) {
                    process_map_hex(hex.Key, color);
                }
            }
        }
    }
}

function Update ()
{

}
