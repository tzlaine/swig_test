#pragma strict

var game_data_ : game_data;

private var place_hexes_ : place_hexes = null;
private var map_hex_height : int = 0;

private static var sin_60 : float = Mathf.Sin(Mathf.Deg2Rad * 60);


function hex_clicked (hc : hex_coord)
{
    print(hc.x + ',' + hc.y);
}

function Start ()
{
    var m : map_t = game_data_.map();
    while (!m) {
        yield WaitForSeconds(0.01);
        m = game_data_.map();
    }

    place_hexes_ = GetComponent(place_hexes);
    map_hex_height = m.hexes.GetLength(1);
}

function OnMouseUpAsButton ()
{
    var box : BoxCollider = GetComponent(BoxCollider);
    var hit : RaycastHit;
    if (box.Raycast(Camera.main.ScreenPointToRay(Input.mousePosition), hit, 1000.0)) {
        var point = hit.point + place_hexes_.map_origin();
        point.x += 1.0;
        point.y += sin_60;

        // "Loose" because the point may fall above the upper-left edge or
        // below the lower-left edge.
        var loose_column = Mathf.FloorToInt(point.x / 1.5);

        if (loose_column % 2 == 1)
            point.y += sin_60;

        var loose_row = Mathf.FloorToInt(point.y / (2 * sin_60));

        var hc : hex_coord = hex_coord(loose_column, map_hex_height - 1 - loose_row);
        if (Mathf.Repeat(point.x, 1.5) < 0.5) {
            var left_corner = Vector2(0, sin_60);
            var left_corner_to_point =
                Vector2(point.x - loose_column * 1.5, point.y - loose_row * 2 * sin_60) -
                left_corner;
            var angle = Vector2.Angle(Vector2.right, left_corner_to_point);
            print(angle);
            if (60.0 < angle) {
                hc = 0 < left_corner_to_point.y ?
                    game_data.hex_coord_above_left(hc) :
                    game_data.hex_coord_below_left(hc);
            }
        }

        hex_clicked(hc);
    }
}
