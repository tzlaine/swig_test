#pragma strict

var hex_border_thickness : float = 0.03;
var province_border_thickness : float = 0.06;
var national_border_thickness : float = 0.12;

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

    var vertices : Vector3[] = new Vector3[24];
    var triangles : int[] = new int[36];
    var normals : Vector3[] = new Vector3[24];
    var uv2 : Vector2[] = new Vector2[24]; // contains (owner, -)

    var i = 0;
    var border_thickness = 0.0;
    for (i = 0; i < 6; ++i) {
        var theta1 : float = Mathf.Deg2Rad * 60 * i;
        var point1 : Vector3 = Vector3(Mathf.Cos(theta1), Mathf.Sin(theta1), 0);
        var theta2 : float = Mathf.Deg2Rad * 60 * (i + 1);
        var point2 : Vector3 = Vector3(Mathf.Cos(theta2), Mathf.Sin(theta2), 0);

        border_thickness =
            adjacencies[i] == 0 ? hex_border_thickness :
            (adjacencies[i] == 1 ? province_border_thickness : national_border_thickness);

        vertices[i * 4 + 0] = point1;
        vertices[i * 4 + 1] = point1 * (1.0 - border_thickness);
        vertices[i * 4 + 2] = point2;
        vertices[i * 4 + 3] = point2 * (1.0 - border_thickness);

        triangles[i * 6 + 0] = i * 4 + 0;
        triangles[i * 6 + 1] = i * 4 + 1;
        triangles[i * 6 + 2] = i * 4 + 3;

        triangles[i * 6 + 3] = i * 4 + 3;
        triangles[i * 6 + 4] = i * 4 + 2;
        triangles[i * 6 + 5] = i * 4 + 0;

        normals[i * 4 + 0] = -Vector3.forward;
        normals[i * 4 + 1] = -Vector3.forward;
        normals[i * 4 + 2] = -Vector3.forward;
        normals[i * 4 + 3] = -Vector3.forward;

        uv2[i * 4 + 0] = Vector2(owner / 255.0, 1);
        uv2[i * 4 + 1] = Vector2(owner / 255.0, 1);
        uv2[i * 4 + 2] = Vector2(owner / 255.0, 1);
        uv2[i * 4 + 3] = Vector2(owner / 255.0, 1);
    }

    for (i = 0; i < 6; ++i) {
        border_thickness =
            adjacencies[i] == 0 ? hex_border_thickness :
            (adjacencies[i] == 1 ? province_border_thickness : national_border_thickness);

        var adj = adjacencies[i];
        var prev_index = (i + 5) % 6;
        var next_index = (i + 1) % 6;
        var prev_adj = adjacencies[prev_index];
        var next_adj = adjacencies[next_index];
        if (prev_adj < adj) {
            var prev_side_direction =
                (vertices[prev_index * 4 + 0] - vertices[prev_index * 4 + 2]).normalized;
            vertices[i * 4 + 1] = vertices[i * 4 + 0] + prev_side_direction * border_thickness;
        }
        if (next_adj < adj) {
            var next_side_direction =
                (vertices[next_index * 4 + 2] - vertices[next_index * 4 + 0]).normalized;
            vertices[i * 4 + 3] = vertices[i * 4 + 2] + next_side_direction * border_thickness;
        }
    }

    mesh.vertices = vertices;
    mesh.triangles = triangles;
    mesh.normals = normals;
    mesh.uv2 = uv2;
}
