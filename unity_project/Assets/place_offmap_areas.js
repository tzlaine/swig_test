#pragma strict

private static var sin_60 : float = Mathf.Sin(Mathf.Deg2Rad * 60);

var offmap_square : GameObject;
var border_square : GameObject;
var text : TextMesh;
var game_data_ : game_data;
var place_hexes_ : place_hexes;
var feature_factory : place_borders_and_features;
var area_thickness : float = 3.0 * sin_60;
var border_thickness : float = 0.12;
var secondary_colors : Texture2D;


function set_owner (mesh : Mesh, owner : int)
{
    var uv2 : Vector2[] = new Vector2[mesh.vertexCount];
    for (var i = 0; i < mesh.vertexCount; ++i) {
        uv2[i] = Vector2(owner / 255.0, 1);
    }
    mesh.uv2 = uv2;
}

// first is left/bottom, second is right/top
function add_offmap (owner : String,
                     owner_id : int,
                     first : hex_coord,
                     second : hex_coord,
                     position : int,
                     name : String,
                     features : String[],
                     m : map_t)
{
    var lower_left : Vector3;
    var upper_right : Vector3;
    var right_hex = m.hexes.GetLength(0) - 1;
    var top_hex = m.hexes.GetLength(1) - 1;
    if (position == 1) { // top
        lower_left = Vector3(
            first.x * 1.5 - 0.5,
            (top_hex - first.y - 1) * 2 * sin_60,
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

    var obj : GameObject = Instantiate(offmap_square);
    var mesh : Mesh = obj.GetComponent(MeshFilter).mesh;
    var vertices : Vector3[] = mesh.vertices;
    for (var i = 0; i < mesh.vertexCount; ++i) {
        vertices[i].x *= size.x;
        vertices[i].y *= size.y;
    }
    mesh.vertices = vertices;
    set_owner(mesh, owner_id);
    obj.renderer.material.renderQueue = 5;
    obj.transform.position =
        (lower_left + upper_right) / 2.0 - place_hexes_.map_origin();

    var triangles : int[] = new int[6];
    triangles[0] = 0;
    triangles[1] = 1;
    triangles[2] = 2;
    triangles[3] = 2;
    triangles[4] = 3;
    triangles[5] = 0;

    // top border
    obj = Instantiate(border_square);
    mesh = obj.GetComponent(MeshFilter).mesh;
    vertices = mesh.vertices;
    vertices[0] = upper_right;
    vertices[1] = Vector3(upper_right.x, upper_right.y - border_thickness, 0);
    vertices[2] = Vector3(lower_left.x, upper_right.y - border_thickness, 0);
    vertices[3] = Vector3(lower_left.x, upper_right.y, 0);
    mesh.vertices = vertices;
    mesh.triangles = triangles;
    set_owner(mesh, owner_id);
    obj.renderer.material.renderQueue = 8;
    obj.transform.position = -place_hexes_.map_origin();

    // bottom border
    obj = Instantiate(border_square);
    mesh = obj.GetComponent(MeshFilter).mesh;
    vertices = mesh.vertices;
    vertices[0] = Vector3(upper_right.x, lower_left.y + border_thickness, 0);
    vertices[1] = Vector3(upper_right.x, lower_left.y, 0);
    vertices[2] = lower_left;
    vertices[3] = Vector3(lower_left.x, lower_left.y + border_thickness, 0);
    mesh.vertices = vertices;
    mesh.triangles = triangles;
    set_owner(mesh, owner_id);
    obj.renderer.material.renderQueue = 8;
    obj.transform.position = -place_hexes_.map_origin();

    // left border
    obj = Instantiate(border_square);
    mesh = obj.GetComponent(MeshFilter).mesh;
    vertices = mesh.vertices;
    vertices[0] = Vector3(lower_left.x + border_thickness, upper_right.y, 0);
    vertices[1] = Vector3(lower_left.x + border_thickness, lower_left.y, 0);
    vertices[2] = lower_left;
    vertices[3] = Vector3(lower_left.x, upper_right.y, 0);
    mesh.vertices = vertices;
    mesh.triangles = triangles;
    set_owner(mesh, owner_id);
    obj.renderer.material.renderQueue = 8;
    obj.transform.position = -place_hexes_.map_origin();

    // left border
    obj = Instantiate(border_square);
    mesh = obj.GetComponent(MeshFilter).mesh;
    vertices = mesh.vertices;
    vertices[0] = upper_right;
    vertices[1] = Vector3(upper_right.x, lower_left.y, 0);
    vertices[2] = Vector3(upper_right.x - border_thickness, lower_left.y, 0);
    vertices[3] = Vector3(upper_right.x - border_thickness, upper_right.y, 0);
    mesh.vertices = vertices;
    mesh.triangles = triangles;
    set_owner(mesh, owner_id);
    obj.renderer.material.renderQueue = 8;
    obj.transform.position = -place_hexes_.map_origin();

    var features_width = 0.0;

    if (features) {
        for (i = 0; i < features.Length; ++i) {
            var start : Vector3;
            var offset : Vector3;
            if (position == 0) { // right
                start = Vector3(upper_right.x - 1.0, lower_left.y + sin_60, 0);
                offset = Vector3(0, 2 * sin_60, 0);
            } else if (position == 1) { // top
                start = Vector3(upper_right.x - 1.0, upper_right.y - sin_60, 0);
                offset = Vector3(1.5, 0, 0);
            } else { // left
                start = Vector3(lower_left.x + 1.0, upper_right.y - sin_60, 0);
                offset = Vector3(0, -2 * sin_60, 0);
            }
            feature_factory.make_feature(
                features[i],
                owner,
                start + i * offset - place_hexes_.map_origin()
            );
            features_width =
                features.Length * Mathf.Abs(position == 1 ? offset.x : offset.y);
        }
    }

    var text_mesh : TextMesh = Instantiate(text);
    text_mesh.text = name;
    text_mesh.color = secondary_colors.GetPixel(owner_id * 4, 0);
    text_mesh.transform.position =
        (lower_left + upper_right) / 2.0 - place_hexes_.map_origin();
    if (position == 0) { // right
        text_mesh.transform.position.x =
            upper_right.x - area_thickness / 3.0 - place_hexes_.map_origin().x;
        text_mesh.transform.position.y += features_width / 4.0;
        text_mesh.transform.localRotation = Quaternion.Euler(0, 0, -90);
    } else if (position == 1) { // top
        text_mesh.transform.position.y =
            upper_right.y - area_thickness / 3.0 - place_hexes_.map_origin().y;
        text_mesh.transform.position.x -= features_width / 4.0;
    } else { // left
        text_mesh.transform.position.x =
            lower_left.x + area_thickness / 3.0 - place_hexes_.map_origin().x;
        text_mesh.transform.position.y -= features_width / 4.0;
        text_mesh.transform.localRotation = Quaternion.Euler(0, 0, 90);
    }
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
        add_offmap(oa.Value.owner,
                   oa.Value.owner_id,
                   oa.Value.position == 1 ?
                       oa.Value.hexes[0] :
                       oa.Value.hexes[oa.Value.hexes.Length - 1],
                   oa.Value.position == 1 ?
                       oa.Value.hexes[oa.Value.hexes.Length - 1] :
                       oa.Value.hexes[0],
                   oa.Value.position,
                   oa.Value.name,
                   oa.Value.features,
                   m);
    }
}
