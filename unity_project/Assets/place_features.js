#pragma strict

var circle : GameObject;
var star : procedural_star;
var place_tiles_ : place_tiles;
var game_data_ : game_data;


function place_features (h : hex, m : map_t)
{
    if (h.feature == 'none')
        return;

    var scale : float = 1.0;

    if (h.feature == 'MIN' || h.feature == 'MAJ') {
        var planet : GameObject = Instantiate(circle);
        planet.transform.position = place_tiles_.hex_center(hex_coord(h.x, h.y));
        scale = h.feature == 'MIN' ? 0.2 : 0.3;
        planet.transform.localScale = Vector3(scale, scale, -scale);
        var mesh : Mesh = planet.GetComponent(MeshFilter).mesh;
        var uv2 : Vector2[] = new Vector2[mesh.vertexCount];
        for (var i = 0; i < mesh.vertexCount; ++i) {
            uv2[i] = Vector2(h.owner_id / 255.0, 1);
        }
        mesh.uv2 = uv2;
        planet.renderer.sharedMaterial.renderQueue = 30;
    } else if (h.feature == 'capitol') {
        var capitol : procedural_star = Instantiate(star);
        capitol.init(h.owner_id, 8);
        capitol.transform.position = place_tiles_.hex_center(hex_coord(h.x, h.y));
        scale = 0.5;
        capitol.transform.localScale = Vector3(scale, scale, scale);
        capitol.renderer.sharedMaterial.renderQueue = 30;
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
