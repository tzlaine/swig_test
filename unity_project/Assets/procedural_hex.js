#pragma strict

private static var vertices : Vector3[];
private static var triangles : int[];
private static var normals : Vector3[];
private static var uv : Vector2[];

function vertex_to_uv (v : Vector3, uv_origin : Vector3, uv_unit : Vector3) : Vector2
{
    var delta : Vector3 = v - uv_origin;
    return Vector2(delta.x / uv_unit.x, delta.y / uv_unit.y);
}

private function build_common_geometry ()
{
    if (vertices)
        return;

    vertices = new Vector3[7];
    triangles = new int[18];
    normals = new Vector3[7];
    uv = new Vector2[7];

    var sin_60 : float = Mathf.Sin(Mathf.Deg2Rad * 60);
    var cos_60 : float = Mathf.Cos(Mathf.Deg2Rad * 60);
    var uv_origin : Vector3 = Vector3(-1, -sin_60, 0);
    var uv_unit : Vector3 = Vector3(cos_60, sin_60, 0) - uv_origin;

    vertices[6] = Vector3(0, 0, 0);
    normals[6] = -Vector3.forward;
    uv[6] = vertex_to_uv(vertices[6], uv_origin, uv_unit);

    for (var i = 0; i < 6; ++i) {
        var theta : float = Mathf.Deg2Rad * 60 * i;
        vertices[i] = Vector3(Mathf.Cos(theta), Mathf.Sin(theta), 0);
        normals[i] = -Vector3.forward;
        triangles[i * 3 + 0] = (i + 1) % 6;
        triangles[i * 3 + 1] = i;
        triangles[i * 3 + 2] = 6;
        uv[i] = vertex_to_uv(vertices[i], uv_origin, uv_unit);
    }
}

function init (owner : int)
{
    var mesh_filter : MeshFilter = GetComponent(MeshFilter);
    var mesh = new Mesh();
    mesh_filter.mesh = mesh;

    build_common_geometry();

    var uv2 : Vector2[] = new Vector2[7]; // contains (owner, -)

    uv2[6] = Vector2(owner / 255.0, 1);

    for (var i = 0; i < 6; ++i) {
        uv2[i] = Vector2(owner / 255.0, 1);
    }

    mesh.vertices = vertices;
    mesh.triangles = triangles;
    mesh.normals = normals;
    mesh.uv = uv;
    mesh.uv2 = uv2;
}
