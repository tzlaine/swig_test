#pragma strict

function vertex_to_uv (v : Vector3, uv_origin : Vector3, uv_unit : Vector3) : Vector2
{
    var delta : Vector3 = v - uv_origin;
    return Vector2(delta.x / uv_unit.x, delta.y / uv_unit.y);
}

// adjacencies are values indicating what is adjacent from that direction.
// The expected values are:
// 0 - same nation, same province
// 1 - same nation, different province
// 2 - different nation
function init (owner : int, adjacencies : int[])
{
    var mesh_filter : MeshFilter = GetComponent(MeshFilter);
    var mesh = new Mesh();
    mesh_filter.mesh = mesh;

    var vertices : Vector3[] = new Vector3[18];
    var triangles : int[] = new int[18];
    var normals : Vector3[] = new Vector3[18];
    var uv : Vector2[] = new Vector2[18];
    var uv2 : Vector2[] = new Vector2[18]; // Contains (gradient, owner).

    var sin_60 : float = Mathf.Sin(Mathf.Deg2Rad * 60);
    var cos_60 : float = Mathf.Cos(Mathf.Deg2Rad * 60);
    var uv_origin : Vector3 = new Vector3(-1, -sin_60, 0);
    var uv_unit : Vector3 = Vector3(cos_60, sin_60, 0) - uv_origin;

    for (var i = 0; i < 6; ++i) {
        var theta1 : float = Mathf.Deg2Rad * 60 * i;
        var theta2 : float = Mathf.Deg2Rad * 60 * (i + 1);

        // Encode the adjacency in z.
        vertices[i * 3 + 0] = new Vector3(Mathf.Cos(theta1), Mathf.Sin(theta1), adjacencies[i]);
        vertices[i * 3 + 1] = new Vector3(Mathf.Cos(theta2), Mathf.Sin(theta2), adjacencies[i]);
        vertices[i * 3 + 2] = new Vector3(0, 0, adjacencies[i]);

        normals[i * 3 + 0] = -Vector3.forward;
        normals[i * 3 + 1] = -Vector3.forward;
        normals[i * 3 + 2] = -Vector3.forward;

        triangles[i * 3 + 0] = i * 3 + 1;
        triangles[i * 3 + 1] = i * 3 + 0;
        triangles[i * 3 + 2] = i * 3 + 2;

        uv[i * 3 + 0] = vertex_to_uv(vertices[i * 3 + 0], uv_origin, uv_unit);
        uv[i * 3 + 1] = vertex_to_uv(vertices[i * 3 + 1], uv_origin, uv_unit);
        uv[i * 3 + 2] = vertex_to_uv(vertices[i * 3 + 2], uv_origin, uv_unit);

        uv2[i * 3 + 0] = new Vector2(1, owner / 255.0);
        uv2[i * 3 + 1] = new Vector2(1, owner / 255.0);
        uv2[i * 3 + 2] = new Vector2(0, owner / 255.0);
    }

    mesh.vertices = vertices;
    mesh.triangles = triangles;
    mesh.normals = normals;
    mesh.uv = uv;
    mesh.uv2 = uv2;
}
