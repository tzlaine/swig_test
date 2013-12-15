#pragma strict

var circle : GameObject;
var place_tiles_ : place_tiles;
var game_data_ : game_data;


function place_features (h : hex, m : map_t)
{
    if (h.feature == 'none')
        return;

    if (h.feature == 'MIN' || h.feature == 'MAJ') {
        var obj : GameObject = Instantiate(circle);
        obj.transform.position = place_tiles_.hex_center(hex_coord(h.x, h.y));
        var scale = h.feature == 'MIN' ? 0.2 : 0.3;
        obj.transform.localScale = Vector3(scale, scale, -scale);
        obj.renderer.material.color =
            game_data_.secondary_color(h.owner); // TODO: Change this to a shader param.
        obj.renderer.material.renderQueue = 30;
    }
}

function Start ()
{
    var m : map_t = game_data_.map();
    while (!m) {
        yield WaitForSeconds(0.01);
        m = game_data_.map();
    }

    for (var x = 0; x < m.hexes.GetLength(0); ++x) {
        for (var y = 0; y < m.hexes.GetLength(1); ++y) {
            place_features(m.hexes[x, y], m);
        }
    }
}

/*
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
	hex_mesh_filter.gameObject.active = false;
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
*/
