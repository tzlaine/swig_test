#pragma strict

var hex_border_gaps : procedural_hex_gaps;
var game_data_ : game_data;
var place_tiles_ : place_tiles;

private var map_width_ : int = 0;
private var map_height_ : int = 0;
private var map_origin : Vector3 = Vector3(0, 0, 0);

private static var sin_60 : float = Mathf.Sin(Mathf.Deg2Rad * 60);


function place_tile (h : hex, m : map_t)
{
    var obj : procedural_hex_gaps = Instantiate(hex_border_gaps);
    var adjacencies_ : int[] = place_tiles_.adjacencies(h, m);
    obj.init(h.owner_id, adjacencies_);
    obj.transform.position =
        Vector3(h.x * 1.5, (map_height_ - 1 - h.y) * 2 * sin_60, 0) - map_origin;
    if (h.x % 2 == 1)
        obj.transform.position.y -= sin_60;
    obj.renderer.material.renderQueue = 20;
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

function Update ()
{
    if (!hexes_combined &&
        place_tiles_.combine_hexes(GetComponent(MeshFilter),
                                   FindObjectsOfType(procedural_hex_gaps))) {
        renderer.material.renderQueue = 20;
        hexes_combined = true;
    }
}
