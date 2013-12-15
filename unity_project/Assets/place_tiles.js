﻿#pragma strict

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

function adjacency (h : hex, h2 : hex) : int
{
    if (!h2 || h.owner_id != h2.owner_id)
        return 2;
    if (h.province != h2.province)
        return 1;
    return 0;
}

function adjacencies (h : hex, m : map_t) : int[]
{
    var retval : int[] = new int[6];
    retval[0] = adjacency(h, game_data.hex_above_right(h, m));
    retval[1] = adjacency(h, game_data.hex_above(h, m));
    retval[2] = adjacency(h, game_data.hex_above_left(h, m));
    retval[3] = adjacency(h, game_data.hex_below_left(h, m));
    retval[4] = adjacency(h, game_data.hex_below(h, m));
    retval[5] = adjacency(h, game_data.hex_below_right(h, m));
    return retval;
}

function place_tile (h : hex, m : map_t)
{
    var obj : procedural_hex = Instantiate(hex_surface);
    var adjacencies_ : int[] = adjacencies(h, m);
    obj.init(h.owner_id, adjacencies_);
    obj.transform.position =
        Vector3(h.x * 1.5, (map_height_ - 1 - h.y) * 2 * sin_60, 0) - map_origin;
    if (h.x % 2 == 1)
        obj.transform.position.y -= sin_60;
    obj.renderer.material.renderQueue = 10;
}

function Start ()
{
    var m : map_t = game_data_.map();

    map_width_ = m.hexes.GetLength(0);
    map_height_ = m.hexes.GetLength(1);

    map_origin = Vector3((map_width_ - 1) * 1.5 / 2,
                         (map_height_ - 1) * 2 * sin_60 / 2,
                         0);

    for (var x = 0; x < m.hexes.GetLength(0); ++x) {
        for (var y = 0; y < m.hexes.GetLength(1); ++y) {
            place_tile(m.hexes[x, y], m);
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

    renderer.material.renderQueue = 10;

    hexes_combined = true;
}

function Update ()
{
    if (!hexes_combined)
        combine_hexes();
}