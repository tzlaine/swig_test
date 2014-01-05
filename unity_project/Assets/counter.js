#pragma strict

@script SerializeAll

@DoNotSerialize
private var game_data_ : game_data;
@DoNotSerialize
private var box_collider_ : BoxCollider;

@DoNotSerialize
var width : float = 0.5;
@DoNotSerialize
var thickness : float = 0.1;

private var counter_ : counter_t = new counter_t();

@DoNotSerialize
private var deserializing : boolean = false;

@DoNotSerialize
private static var vertices : Vector3[];
@DoNotSerialize
private static var quads : int[];
@DoNotSerialize
private static var normals : Vector3[];

function Awake ()
{
    game_data_ = FindObjectOfType(game_data);
    box_collider_ = GetComponent(BoxCollider);
    box_collider_.enabled = false;
    if (JSONLevelSerializer.IsDeserializing)
        deserializing = true;
}

function Update ()
{
    if (deserializing && !JSONLevelSerializer.IsDeserializing) {
        build_mesh();
        deserializing = false;
        enabled = false;
    }

    if (!deserializing)
        enabled = false;
}

// TODO
//function flip ()
//{ /* TODO: Tween! */ }

// TODO: Make shared mesh geometry (verts, normals, uvs) static in other
// scripts.
// TODO: Make sure other empty-Update() scripts are set to enabled=false.

function init (nation : String, unit : String, crippled : boolean)
{
    var c = game_data_.counter(nation, unit);
    counter_.uncrippled_side = new counter_side_t(c.uncrippled_side);
    counter_.uncrippled_side.texture = null;
    counter_.crippled_side = new counter_side_t(c.crippled_side);
    counter_.crippled_side.texture = null;

    if (crippled)
        transform.localRotation = Quaternion.Euler(0, 180, 0);

    build_mesh();
}

private function texture_index (texture_filename : String)
{
    var retval = 0;
    if (texture_filename.Contains('alpha_front'))
        retval = 0;
    else if (texture_filename.Contains('alpha_back'))
        retval = 1;
    else if (texture_filename.Contains('bravo_front'))
        retval = 2;
    else if (texture_filename.Contains('bravo_back'))
        retval = 3;
    else if (texture_filename.Contains('bases_front'))
        retval = 4;
    else if (texture_filename.Contains('bases_back'))
        retval = 5;
    else if (texture_filename.Contains('counters_front'))
        retval = 6;
    return retval;
}

private function build_common_geometry ()
{
    if (vertices != null)
        return;

    vertices = new Vector3[24];
    quads = new int[24];
    normals = new Vector3[24];

    // front face
    vertices[0] = Vector3(-width, -width, -thickness);
    vertices[1] = Vector3(-width,  width, -thickness);
    vertices[2] = Vector3( width,  width, -thickness);
    vertices[3] = Vector3( width, -width, -thickness);
    normals[0] = Vector3(0, 0, -1);
    normals[1] = Vector3(0, 0, -1);
    normals[2] = Vector3(0, 0, -1);
    normals[3] = Vector3(0, 0, -1);

    // back face
    vertices[4] = Vector3( width, -width, 0.0);
    vertices[5] = Vector3( width,  width, 0.0);
    vertices[6] = Vector3(-width,  width, 0.0);
    vertices[7] = Vector3(-width, -width, 0.0);
    normals[4] = Vector3(0, 0, 1);
    normals[5] = Vector3(0, 0, 1);
    normals[6] = Vector3(0, 0, 1);
    normals[7] = Vector3(0, 0, 1);

    // +y side
    vertices[8] = vertices[1];
    vertices[9] = vertices[6];
    vertices[10] = vertices[5];
    vertices[11] = vertices[2];
    normals[8] = Vector3(0, 1, 0);
    normals[9] = Vector3(0, 1, 0);
    normals[10] = Vector3(0, 1, 0);
    normals[11] = Vector3(0, 1, 0);

    // -y side
    vertices[12] = vertices[0];
    vertices[13] = vertices[3];
    vertices[14] = vertices[4];
    vertices[15] = vertices[7];
    normals[12] = Vector3(0, -1, 0);
    normals[13] = Vector3(0, -1, 0);
    normals[14] = Vector3(0, -1, 0);
    normals[15] = Vector3(0, -1, 0);

    // +x side
    vertices[16] = vertices[3];
    vertices[17] = vertices[2];
    vertices[18] = vertices[5];
    vertices[19] = vertices[4];
    normals[16] = Vector3(1, 0, 0);
    normals[17] = Vector3(1, 0, 0);
    normals[18] = Vector3(1, 0, 0);
    normals[19] = Vector3(1, 0, 0);

    // -x side
    vertices[20] = vertices[7];
    vertices[21] = vertices[6];
    vertices[22] = vertices[1];
    vertices[23] = vertices[0];
    normals[20] = Vector3(-1, 0, 0);
    normals[21] = Vector3(-1, 0, 0);
    normals[22] = Vector3(-1, 0, 0);
    normals[23] = Vector3(-1, 0, 0);

    for (var i = 0; i < quads.Length; ++i) {
        quads[i] = i;
    }
}

private function build_mesh ()
{
    var mesh_filter : MeshFilter = GetComponent(MeshFilter);
    var mesh = new Mesh();
    mesh_filter.mesh = mesh;

    build_common_geometry();

    var uv : Vector2[] = new Vector2[24];
    var uv2 : Vector2[] = new Vector2[24]; // contains (texture-id, -)

    var i = 0;

    for (i = 0; i < uv.Length / 4; ++i) {
        var uv_min = Vector2(0, 0);
        var uv_max = Vector2(1, 1);
        if (i == 0) {
            uv_min = counter_.uncrippled_side.uv_min;
            uv_max = counter_.uncrippled_side.uv_max;
        } else if (i == 1) {
            uv_min = counter_.crippled_side.uv_min;
            uv_max = counter_.crippled_side.uv_max;
        }
        uv[i * 4 + 0] = uv_min;
        uv[i * 4 + 1] = Vector2(uv_min.x, uv_max.y);
        uv[i * 4 + 2] = uv_max;
        uv[i * 4 + 3] = Vector2(uv_max.x, uv_min.y);
    }

    var uncrippled = texture_index(counter_.uncrippled_side.texture_filename);
    var crippled = texture_index(counter_.crippled_side.texture_filename);

    for (i = 0; i < uv2.Length; ++i) {
        if (i < 4)
            uv2[i] = Vector2(uncrippled, 0);
        else if (i < 8)
            uv2[i] = Vector2(crippled, 0);
        else
            uv2[i] = Vector2(6, 0); // counters_front
    }

    mesh.vertices = vertices;
    mesh.SetIndices(quads, MeshTopology.Quads, 0);
    mesh.normals = normals;
    mesh.uv = uv;
    mesh.uv2 = uv2;

    mesh.RecalculateBounds();
}
