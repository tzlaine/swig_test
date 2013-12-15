#pragma strict

function vertex_to_uv (v : Vector3, uv_origin : Vector3, uv_unit : Vector3) : Vector2
{
    var delta : Vector3 = v - uv_origin;
    return Vector2(delta.x / uv_unit.x, delta.y / uv_unit.y);
}

// Gap types are values indicating what type of border contains a gap.
// The expected values are:
// 0 - same nation, same province
// 1 - same nation, different province
// 2 - different nation
function init (owner : int, total_gaps : int, gaps : List.<border_gap>)
{
    var mesh_filter : MeshFilter = GetComponent(MeshFilter);
    var mesh = new Mesh();
    mesh_filter.mesh = mesh;

    var vertex_count : int = total_gaps * 3;

    var vertices : Vector3[] = new Vector3[vertex_count];
    var triangles : int[] = new int[vertex_count];
    var normals : Vector3[] = new Vector3[vertex_count];
    var uv : Vector2[] = new Vector2[vertex_count]; // Contains (gradient, owner).

    var triangle_index : int = 0;
    for (var i = 0; i < gaps.Count; ++i) {
        var theta1 : float = Mathf.Deg2Rad * 60 * gaps[i].edge;
        var theta2 : float = Mathf.Deg2Rad * 60 * (gaps[i].edge + 1);

        var triangle : Vector3[] = [
            Vector3(Mathf.Cos(theta1), Mathf.Sin(theta1), 0),
            Vector3(Mathf.Cos(theta2), Mathf.Sin(theta2), 0),
            Vector3(0, 0, 0)
        ];

        if (gaps[i].first_vertex) {
            vertices[triangle_index * 3 + 0] = triangle[0];
            vertices[triangle_index * 3 + 1] = triangle[1];
            vertices[triangle_index * 3 + 2] = triangle[2];

            vertices[triangle_index * 3 + 0].z = gaps[i].first_gap_type;
            vertices[triangle_index * 3 + 1].z = gaps[i].first_gap_type;
            vertices[triangle_index * 3 + 2].z = gaps[i].first_gap_type;

            normals[triangle_index * 3 + 0] = -Vector3.forward;
            normals[triangle_index * 3 + 1] = -Vector3.forward;
            normals[triangle_index * 3 + 2] = -Vector3.forward;

            triangles[triangle_index * 3 + 0] = triangle_index * 3 + 1;
            triangles[triangle_index * 3 + 1] = triangle_index * 3 + 0;
            triangles[triangle_index * 3 + 2] = triangle_index * 3 + 2;

            uv[triangle_index * 3 + 0] = new Vector2(1, owner / 255.0);
            uv[triangle_index * 3 + 1] = new Vector2(0, owner / 255.0);
            uv[triangle_index * 3 + 2] = new Vector2(0, owner / 255.0);

            ++triangle_index;
        }

        if (gaps[i].second_vertex) {
            vertices[triangle_index * 3 + 0] = triangle[0];
            vertices[triangle_index * 3 + 1] = triangle[1];
            vertices[triangle_index * 3 + 2] = triangle[2];

            vertices[triangle_index * 3 + 0].z = gaps[i].second_gap_type;
            vertices[triangle_index * 3 + 1].z = gaps[i].second_gap_type;
            vertices[triangle_index * 3 + 2].z = gaps[i].second_gap_type;

            normals[triangle_index * 3 + 0] = -Vector3.forward;
            normals[triangle_index * 3 + 1] = -Vector3.forward;
            normals[triangle_index * 3 + 2] = -Vector3.forward;

            triangles[triangle_index * 3 + 0] = triangle_index * 3 + 1;
            triangles[triangle_index * 3 + 1] = triangle_index * 3 + 0;
            triangles[triangle_index * 3 + 2] = triangle_index * 3 + 2;

            uv[triangle_index * 3 + 0] = new Vector2(0, owner / 255.0);
            uv[triangle_index * 3 + 1] = new Vector2(1, owner / 255.0);
            uv[triangle_index * 3 + 2] = new Vector2(0, owner / 255.0);

            ++triangle_index;
        }
    }

    mesh.vertices = vertices;
    mesh.triangles = triangles;
    mesh.normals = normals;
    mesh.uv = uv;
}
