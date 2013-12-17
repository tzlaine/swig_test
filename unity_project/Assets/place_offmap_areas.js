#pragma strict

var square : GameObject;
var game_data_ : game_data;
var place_hexes_ : place_hexes;

private static var sin_60 : float = Mathf.Sin(Mathf.Deg2Rad * 60);


function set_owner (mesh : Mesh, owner : int)
{
    var uv2 : Vector2[] = new Vector2[mesh.vertexCount];
    for (var i = 0; i < mesh.vertexCount; ++i) {
        uv2[i] = Vector2(owner / 255.0, 1);
    }
    mesh.uv2 = uv2;
}

// first is left/bottom, second is right/top
function add_offmap (owner : int,
                     first : hex_coord,
                     second : hex_coord,
                     position : int,
                     m : map_t)
{
    var area_thickness = 3.0 * sin_60;

    var lower_left : Vector3;
    var upper_right : Vector3;
    var right_hex = m.hexes.GetLength(0) - 1;
    var top_hex = m.hexes.GetLength(1) - 1;
    if (position == 1) { // top
        lower_left = Vector3(
            first.x * 1.5 - 0.5,
            (top_hex - first.y) * 2 * sin_60,
            0
        );

        upper_right = Vector3(
            second.x * 1.5 + 0.5,
            (top_hex - second.y) * 2 * sin_60 + area_thickness,
            0
        );
        if (second.x % 2 == 1)
            upper_right.y += sin_60;
    } else { // left or right
        lower_left = Vector3(
            position == 0 ?
                right_hex * 1.5 - 2 * area_thickness :
                -area_thickness,
            (top_hex - first.y) * 2 * sin_60,
            0
        );
        if (first.x % 2 == 1)
            lower_left.y -= sin_60;

        upper_right = Vector3(
            position == 0 ?
                right_hex * 1.5 + area_thickness :
                2 * area_thickness,
            (top_hex - second.y) * 2 * sin_60,
            0
        );
        if (second.x % 2 == 1)
            upper_right.y -= sin_60;

        if (first.y - second.y < 5) {
            lower_left.y -= sin_60;
            upper_right.y += sin_60;
        }
    }

    var size = upper_right - lower_left;
    var obj : GameObject = Instantiate(square);
    var mesh : Mesh = obj.GetComponent(MeshFilter).mesh;
    var vertices : Vector3[] = mesh.vertices;
    for (var i = 0; i < mesh.vertexCount; ++i) {
        vertices[i].x *= size.x;
        vertices[i].y *= size.y;
    }
    mesh.vertices = vertices;
    set_owner(mesh, owner);
    obj.renderer.material.renderQueue = 5;// TODO: sharedMaterial?
    obj.transform.position =
        (lower_left + upper_right) / 2.0 - place_hexes_.map_origin();
}

function Start ()
{
    var m : map_t = game_data_.map();
    while (!m) {
        yield WaitForSeconds(0.01);
        m = game_data_.map();
    }

    for (var oa : System.Collections.Generic.KeyValuePair.<String, offmap_area_t> in
         m.offmap_areas) {
        add_offmap(oa.Value.owner_id,
                   oa.Value.position == 1 ?
                       oa.Value.hexes[0] :
                       oa.Value.hexes[oa.Value.hexes.Length - 1],
                   oa.Value.position == 1 ?
                       oa.Value.hexes[oa.Value.hexes.Length - 1] :
                       oa.Value.hexes[0],
                   oa.Value.position,
                   m);
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
*/
