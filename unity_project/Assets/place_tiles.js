#pragma strict

import SimpleJSON;
import System.IO;

var hex_surface : procedural_hex;
var game_data_ : game_data;

private var map_width_ : int = 0;
private var map_height_ : int = 0;
private var map_origin : Vector3 = Vector3(0, 0, 0);

private static var sin_60 : float = Mathf.Sin(Mathf.Deg2Rad * 60);


function map_width ()
{ return map_origin.x * 2; }

function map_height ()
{ return map_origin.y * 2; }


function place_tile (h : hex)
{
    var obj : procedural_hex = Instantiate(hex_surface);
    obj.init(h.owner_id);
    obj.transform.position =
        Vector3(h.x * 1.5, (map_height_ - 1 - h.y) * 2 * sin_60, 0) - map_origin;
    if (h.x % 2 == 1)
        obj.transform.position.y -= sin_60;
}

function Start ()
{
    var m : map_t = game_data_.map();

    // Get map data.
    var json = JSON.Parse(System.IO.File.ReadAllText('../map.json'));

    map_width_ = json['width'].AsInt;
    map_height_ = json['height'].AsInt;
    map_origin = Vector3((m.hexes.GetLength(0) - 1) * 1.5 / 2,
                         (m.hexes.GetLength(1) - 1) * 2 * sin_60 / 2,
                         0);

    for (var x = 0; x < m.hexes.GetLength(0); ++x) {
        for (var y = 0; y < m.hexes.GetLength(1); ++y) {
            place_tile(m.hexes[x, y]);
        }
    }
}

private var hexes_combined = false;

function combine_hexes ()
{
    var mesh_filter : MeshFilter = GetComponent(MeshFilter);
    var procedural_hexes : procedural_hex[] = FindObjectsOfType(procedural_hex);

    if (!procedural_hexes.Length)
        return;

    var mesh = new Mesh();
    mesh_filter.mesh = mesh;

    var combine : CombineInstance[] = new CombineInstance[procedural_hexes.Length];
    for (var i = 0; i < procedural_hexes.Length; i++){
        var hex_mesh_filter : MeshFilter = procedural_hexes[i].GetComponent(MeshFilter);
	combine[i].mesh = hex_mesh_filter.mesh;
	combine[i].transform = hex_mesh_filter.transform.localToWorldMatrix;
	hex_mesh_filter.gameObject.active = false;
    }
    mesh.CombineMeshes(combine);

    hexes_combined = true;
}

function Update ()
{
    if (!hexes_combined)
        combine_hexes();
}
