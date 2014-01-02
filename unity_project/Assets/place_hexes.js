#pragma strict

var hex_surface : procedural_hex;
var game_data_ : game_data;
var highlight : GameObject;
var secondary_colors : Texture2D;

private var map_width_ : int = 0;
private var map_height_ : int = 0;
private var map_origin_ : Vector3 = Vector3(0, 0, 0);

private static var sin_60 : float = Mathf.Sin(Mathf.Deg2Rad * 60);

private var highlighting : GameObject[,];
private var pending_highlights : hex_coord[];


function hex_center (hc : hex_coord)
{
    var retval =
        Vector3(hc.x * 1.5, (map_height_ - 1 - hc.y) * 2 * sin_60, 0) -
        map_origin_;
    if (hc.x % 2 == 1)
        retval.y -= sin_60;
    return retval;
 }

function map_width ()
{ return map_origin_.x * 2; }

function map_height ()
{ return map_origin_.y * 2; }

function map_origin ()
{ return map_origin_; }

function place_tile (h : hex_t, m : map_t)
{
    var surface : procedural_hex = Instantiate(hex_surface);
    surface.init(h.owner_id);
    surface.transform.position = hex_center(h.hc);
    surface.renderer.material.renderQueue = 10;

    // TODO: Optimize via procedural assignment of owner ID in mesh uv2, as
    // necessary.
    var hilite = Instantiate(highlight);
    hilite.transform.position = hex_center(h.hc);
    hilite.renderer.material.renderQueue = 100;
    hilite.renderer.material.color =
        secondary_colors.GetPixel(h.owner_id * 4, 0);
    hilite.SetActive(h.highlight);
    highlighting[h.hc.x, h.hc.y] = hilite;
}

function clear_highlighting ()
{
    var m : map_t = game_data_.map();
    if (m && highlighting) {
        for (var x = 0; x < m.width; ++x) {
            for (var y = 0; y < m.height; ++y) {
                var hex : hex_t = m.hex(hex_coord(x, y));
                hex.highlight = false;
                highlighting[x, y].SetActive(false);
            }
        }
    }
}

function highlight_hexes (nation : String, fleet : String)
{ highlight_hexes(game_data_.nation(nation).starting_forces[fleet].area); }

function highlight_hexes (hexes : hex_coord[])
{
    var m : map_t = game_data_.map();
    if (m && highlighting) {
        for (hc in hexes) {
            if (hc.x != hex_coord().x || hc.y != hex_coord().y) {
                var hex : hex_t = m.hex(hc);
                hex.highlight = true;
                highlighting[hc.x, hc.y].SetActive(true);
            }
        }
    } else {
        pending_highlights = hexes;
    }
}

function highlighted (hc : hex_coord)
{ return highlighting && highlighting[hc.x, hc.y].activeSelf; }

function Start ()
{
    var m : map_t = game_data_.map();
    while (!m) {
        yield WaitForSeconds(0.01);
        m = game_data_.map();
    }

    map_width_ = m.width;
    map_height_ = m.height;

    map_origin_ = Vector3((map_width_ - 1) * 1.5 / 2,
                          (map_height_ - 1) * 2 * sin_60 / 2,
                          0);

    highlighting = new GameObject[m.width, m.height];

    for (var x = 0; x < m.width; ++x) {
        for (var y = 0; y < m.height; ++y) {
            place_tile(m.hex(hex_coord(x, y)), m);
        }
    }

    if (pending_highlights) {
        highlight_hexes(pending_highlights);
        pending_highlights = null;
    }
}

private var hexes_combined = false;

function combine_hexes (mesh_filter : MeshFilter, components : Component[]) : boolean
{
    if (!components.Length)
        return false;

    var mesh = new Mesh();
    mesh_filter.mesh = mesh;

    var combine : CombineInstance[] = new CombineInstance[components.Length];
    for (var i = 0; i < components.Length; i++){
        var hex_mesh_filter : MeshFilter = components[i].GetComponent(MeshFilter);
	combine[i].mesh = hex_mesh_filter.mesh;
	combine[i].transform = hex_mesh_filter.transform.localToWorldMatrix;
	hex_mesh_filter.gameObject.SetActive(false);
    }
    mesh.CombineMeshes(combine);

    return true;
}

function Update ()
{
    if (!hexes_combined &&
        combine_hexes(GetComponent(MeshFilter),
                      FindObjectsOfType(procedural_hex))) {
        renderer.material.renderQueue = 10;
        hexes_combined = true;
    }
}
