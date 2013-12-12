#pragma strict

function vertex_to_uv (v : Vector3, uv_origin : Vector3, uv_unit : Vector3) : Vector2
{
    var delta : Vector3 = v - uv_origin;
    return Vector2(delta.x / uv_unit.x, delta.y / uv_unit.y);
}

function init (owner : int)
{
    var mesh_filter : MeshFilter = GetComponent(MeshFilter);
    var mesh = new Mesh();
    mesh_filter.mesh = mesh;

    var vertices : Vector3[] = new Vector3[7];
    var triangles : int[] = new int[3 * 6];
    var normals : Vector3[] = new Vector3[7];
    var uv : Vector2[] = new Vector2[7];
    var uv2 : Vector2[] = new Vector2[7];

    // Encode the ownership of the surrounding hexes into the colors and
    // tangents.
    var neighbors_1 : Color[] = new Color[7];
    var neighbors_2 : Vector4[] = new Vector4[7];

    var sin_60 : float = Mathf.Sin(Mathf.Deg2Rad * 60);
    var cos_60 : float = Mathf.Cos(Mathf.Deg2Rad * 60);
    var uv_origin : Vector3 = new Vector3(-1, -sin_60, 0);
    var uv_unit : Vector3 = Vector3(cos_60, sin_60, 0) - uv_origin;

    vertices[6] = new Vector3(0, 0, 0);
    normals[6] = -Vector3.forward;
    uv[6] = vertex_to_uv(vertices[6], uv_origin, uv_unit);
    uv2[6] = new Vector2(0, 0);
    neighbors_1[6] = Color(owner / 255.0, owner / 255.0, owner / 255.0, owner / 255.0);
    neighbors_2[6] = Vector4(owner / 255.0, owner / 255.0, owner / 255.0, 1);

    for (var i = 0; i < 6; ++i) {
        var theta : float = Mathf.Deg2Rad * 60 * i;
        // Encode the vertex index in z.
        vertices[i] = new Vector3(Mathf.Cos(theta), Mathf.Sin(theta), i);
        normals[i] = -Vector3.forward;
        triangles[i * 3 + 0] = (i + 1) % 6;
        triangles[i * 3 + 1] = i;
        triangles[i * 3 + 2] = 6;
        uv[i] = vertex_to_uv(vertices[i], uv_origin, uv_unit);
        uv2[i] = new Vector2(1, 0);

        // TODO: Use real values.
        neighbors_1[i] = Color(owner / 255.0, owner / 255.0, owner / 255.0, 1);
        neighbors_2[i] = Vector4(owner / 255.0, owner / 255.0, owner / 255.0, 1);
    }

    mesh.vertices = vertices;
    mesh.triangles = triangles;
    mesh.normals = normals;
    mesh.uv = uv;
    mesh.uv2 = uv2;

    mesh.colors = neighbors_1;
    mesh.tangents = neighbors_2;
}
