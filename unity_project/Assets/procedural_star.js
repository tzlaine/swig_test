#pragma strict

function init (owner : int, points : int)
{
    var mesh_filter : MeshFilter = GetComponent(MeshFilter);
    var mesh = new Mesh();
    mesh_filter.mesh = mesh;

    var num_vertices  = points * 2 + 1;
    var last_vertex = num_vertices - 1;
    var noncenter_vertices = num_vertices - 1;
    var num_triangles  = points * 6;

    var vertices : Vector3[] = new Vector3[num_vertices];
    var triangles : int[] = new int[num_triangles];
    var normals : Vector3[] = new Vector3[num_vertices];
    var uv2 : Vector2[] = new Vector2[num_vertices]; // Contains (owner, -).

    vertices[last_vertex] = Vector3(0, 0, 0);
    normals[last_vertex] = -Vector3.forward;
    uv2[last_vertex] = Vector2(owner / 255.0, 0);

    var corner_offset = 0;
    var point_offset = 0;
    var inner_radius = 0.5;

    if (points == 5) {
        corner_offset = -90;
        point_offset = 360.0 / (points * 2) - 90;
        inner_radius = 0.38667;
    } else if (points == 6) {
        corner_offset = 0;
        point_offset = 360.0 / (points * 2);
        inner_radius = 0.58667;
    } else if (points == 8) {
        corner_offset = 0;
        point_offset = 360.0 / (points * 2);
        inner_radius = 0.54667;
    }

    var i = 0;
    for (i = 0; i < points; ++i) {
        var corner_theta = Mathf.Deg2Rad * (corner_offset + 360.0 / points * i);
        var point_theta = Mathf.Deg2Rad * (point_offset + 360.0 / points * i);
        vertices[i * 2 + 0] =
            Vector3(Mathf.Cos(corner_theta), Mathf.Sin(corner_theta), 0) * inner_radius;
        vertices[i * 2 + 1] =
            Vector3(Mathf.Cos(point_theta), Mathf.Sin(point_theta), 0);
        normals[i * 2 + 0] = -Vector3.forward;
        normals[i * 2 + 1] = -Vector3.forward;
        uv2[i * 2 + 0] = uv2[last_vertex];
        uv2[i * 2 + 1] = uv2[last_vertex];
    }

    for (i = 0; i < points; ++i) {
        triangles[i * 6 + 0] = (i * 2 + 2) % noncenter_vertices;
        triangles[i * 6 + 1] = i * 2 + 1;
        triangles[i * 6 + 2] = i * 2 + 0;

        triangles[i * 6 + 3] = (i * 2 + 2) % noncenter_vertices;
        triangles[i * 6 + 4] = i * 2 + 0;
        triangles[i * 6 + 5] = last_vertex;
    }

    mesh.vertices = vertices;
    mesh.triangles = triangles;
    mesh.normals = normals;
    mesh.uv2 = uv2;
}

function Awake ()
{ enabled = false; }
