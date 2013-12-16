#pragma strict

function init (owner : int, points : int)
{
    var mesh_filter : MeshFilter = GetComponent(MeshFilter);
    var mesh = new Mesh();
    mesh_filter.mesh = mesh;

    var vertices : Vector3[] = null;
    var triangles : int[] = null;
    var normals : Vector3[] = null;
    var uv2 : Vector2[] = null;
    var i = 0;

    if (points == 8) {
        vertices = new Vector3[17];
        triangles = new int[48];
        normals = new Vector3[17];
        uv2 = new Vector2[17]; // Contains (owner, -).

        vertices[16] = Vector3(0, 0, 0);
        normals[16] = -Vector3.forward;
        uv2[16] = Vector2(owner / 255.0, 0);

        for (i = 0; i < 8; ++i) {
            var corner_theta = Mathf.Deg2Rad * (360.0 / 8 * i);
            var point_theta = Mathf.Deg2Rad * (360.0 / 16 + 360.0 / 8 * i);
            vertices[i * 2 + 0] =
                Vector3(Mathf.Cos(corner_theta), Mathf.Sin(corner_theta), 0) * 164.0 / 300.0;
            vertices[i * 2 + 1] =
                Vector3(Mathf.Cos(point_theta), Mathf.Sin(point_theta), 0);
            normals[i * 2 + 0] = -Vector3.forward;
            normals[i * 2 + 1] = -Vector3.forward;
            uv2[i * 2 + 0] = uv2[16];
            uv2[i * 2 + 1] = uv2[16];
        }

        for (i = 0; i < 8; ++i) {
            triangles[i * 6 + 0] = (i * 2 + 2) % 16;
            triangles[i * 6 + 1] = i * 2 + 1;
            triangles[i * 6 + 2] = i * 2 + 0;

            triangles[i * 6 + 3] = (i * 2 + 2) % 16;
            triangles[i * 6 + 4] = i * 2 + 0;
            triangles[i * 6 + 5] = 16;
        }
    }

    mesh.vertices = vertices;
    mesh.triangles = triangles;
    mesh.normals = normals;
    mesh.uv2 = uv2;
}
