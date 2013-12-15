﻿#pragma strict

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
    var uv2 : Vector2[] = new Vector2[18];

    // Encode the ownership of the surrounding hexes into the colors and
    // tangents.
    var neighbors_1 : Color[] = new Color[18];
    var neighbors_2 : Vector4[] = new Vector4[18];

    var sin_60 : float = Mathf.Sin(Mathf.Deg2Rad * 60);
    var cos_60 : float = Mathf.Cos(Mathf.Deg2Rad * 60);
    var uv_origin : Vector3 = new Vector3(-1, -sin_60, 0);
    var uv_unit : Vector3 = Vector3(cos_60, sin_60, 0) - uv_origin;

    neighbors_1[17] = Color(owner / 255.0, adjacencies[0] / 2.0, adjacencies[1] / 2.0, adjacencies[2] / 2.0);
    neighbors_2[17] = Vector4(adjacencies[3] / 2.0, adjacencies[4] / 2.0, adjacencies[5] / 2.0, 1);

    for (var i = 0; i < 6; ++i) {
        var theta1 : float = Mathf.Deg2Rad * 60 * i;
        var theta2 : float = Mathf.Deg2Rad * 60 * (i + 1);

        // Encode the edge index in z.
        vertices[i * 3 + 0] = new Vector3(Mathf.Cos(theta1), Mathf.Sin(theta1), i);
        vertices[i * 3 + 1] = new Vector3(Mathf.Cos(theta2), Mathf.Sin(theta2), i);
        vertices[i * 3 + 2] = new Vector3(0, 0, i);

        normals[i * 3 + 0] = -Vector3.forward;
        normals[i * 3 + 1] = -Vector3.forward;
        normals[i * 3 + 2] = -Vector3.forward;

        triangles[i * 3 + 0] = i * 3 + 1;
        triangles[i * 3 + 1] = i * 3 + 0;
        triangles[i * 3 + 2] = i * 3 + 2;

        uv[i * 3 + 0] = vertex_to_uv(vertices[i * 3 + 0], uv_origin, uv_unit);
        uv[i * 3 + 1] = vertex_to_uv(vertices[i * 3 + 1], uv_origin, uv_unit);
        uv[i * 3 + 2] = vertex_to_uv(vertices[i * 3 + 2], uv_origin, uv_unit);

        uv2[i * 3 + 0] = new Vector2(1, 0);
        uv2[i * 3 + 1] = new Vector2(1, 0);
        uv2[i * 3 + 2] = new Vector2(0, 0);

        neighbors_1[i * 3 + 0] = neighbors_1[17];
        neighbors_1[i * 3 + 1] = neighbors_1[17];
        neighbors_1[i * 3 + 2] = neighbors_1[17];

        neighbors_2[i * 3 + 0] = neighbors_2[17];
        neighbors_2[i * 3 + 1] = neighbors_2[17];
        neighbors_2[i * 3 + 2] = neighbors_2[17];
    }

    mesh.vertices = vertices;
    mesh.triangles = triangles;
    mesh.normals = normals;
    mesh.uv = uv;
    mesh.uv2 = uv2;

    mesh.colors = neighbors_1;
    mesh.tangents = neighbors_2;
}