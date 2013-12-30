#pragma strict

var game_data_ : game_data;
var camera_controller_ : camera_controller;
var drag_sensitivity : float = 0.1;

private var place_hexes_ : place_hexes = null;
private var map_box : BoxCollider = null;
private var map_hex_width : int = 0;
private var map_hex_height : int = 0;
private var can_drag = true;
private var have_drag_origin = false;
private var min_drag_distance = 5.0;
private var min_drag_time = 0.1;
private var dragging = false;
private var mouse_drag_origin = Vector2(0, 0);
private var drag_start_time = 0.0;
private var drag_start_anchor = Vector3(0, 0, 0);
private var offmap_areas : Dictionary.<String, Rect> = new Dictionary.<String, Rect>();

private static var sin_60 : float = Mathf.Sin(Mathf.Deg2Rad * 60);


function add_offmap_area (owner : String, lower_left : Vector3, upper_right : Vector3)
{
    offmap_areas[owner] =
        Rect(lower_left.x,
             lower_left.y,
             upper_right.x - lower_left.x,
             upper_right.y - lower_left.y);
}

function offmap_area_under_cursor () : String
{
    var retval = '';
    var hit : RaycastHit;
    if (map_box.Raycast(Camera.main.ScreenPointToRay(Input.mousePosition), hit, 1000.0)) {
        for (var offmap : System.Collections.Generic.KeyValuePair.<String, Rect> in
             offmap_areas) {
            if (offmap.Value.Contains(hit.point)) {
                retval = offmap.Key;
                break;
            }
        }
    }
    return retval;
}

function hex_under_cursor () : hex_coord
{
    var retval = hex_coord();

    var hit : RaycastHit;
    if (map_box.Raycast(Camera.main.ScreenPointToRay(Input.mousePosition), hit, 1000.0)) {
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
            if (60.0 < angle) {
                hc = 0 < left_corner_to_point.y ?
                    game_data.hex_coord_above_left(hc) :
                    game_data.hex_coord_below_left(hc);
            }
        }

        retval = hc;
    }

    if (retval.x < 0 || map_hex_width <= retval.x ||
        retval.y < 0 || map_hex_height <= retval.y) {
        retval = hex_coord();
    }

    return retval;
}

function Start ()
{
    var m : map_t = game_data_.map();
    while (!m) {
        yield WaitForSeconds(0.01);
        m = game_data_.map();
    }

    place_hexes_ = GetComponent(place_hexes);
    map_box = GetComponent(BoxCollider);
    map_hex_width = m.width;
    map_hex_height = m.height;
}

function Update ()
{
    if (!Input.GetMouseButton(0)) {
        can_drag = true;
        have_drag_origin = false;
        dragging = false;
    }
}

function OnMouseUpAsButton ()
{
    if (can_drag && !dragging) {
        var hc = hex_under_cursor();
        if (hc.x == hex_coord().x && hc.y == hex_coord().y) {
            var offmap_owner = offmap_area_under_cursor();
            print('offmap for owner ' + offmap_owner);
        } else {
            var hc2 : hex_coord = hex_coord(hc.x + 1, hc.y + 1);
            print('hex ' + (hc2.x < 10 ? '0' : '') + (hc2.x * 100 + hc2.y) +
                  ' (' + hc.x + ',' + hc.y + ')');
        }
    }
}

function OnMouseDrag ()
{
    if (can_drag && !dragging) {
        if (have_drag_origin) {
            if (!dragging) {
                var distance = (Input.mousePosition - mouse_drag_origin).magnitude;
                var delta_t = Time.time - drag_start_time;
                if (min_drag_distance < distance && min_drag_time < drag_start_time)
                    dragging = true;
            }
        } else {
            var hc = hex_under_cursor();
            if (hc.x == hex_coord().x && hc.y == hex_coord().y) {
                var offmap_owner = offmap_area_under_cursor();
                if (offmap_owner == '')
                    can_drag = false;
            }
            if (can_drag) {
                have_drag_origin = true;
                mouse_drag_origin = Input.mousePosition;
                drag_start_anchor = camera_controller_.anchor();
                drag_start_time = Time.time;
            }
        }
    }

    if (dragging) {
        var world_up = Camera.main.transform.TransformDirection(Vector3.up);
        var world_left = Camera.main.transform.TransformDirection(Vector3.left);

        var up_projected_on_map = Vector3.Cross(world_left, Vector3.forward);
        var left_projected_on_map = Vector3.Cross(-Vector3.forward, world_up);

        var mouse_delta = mouse_drag_origin - Input.mousePosition;
        var delta =
            mouse_delta.x * left_projected_on_map +
            mouse_delta.y * up_projected_on_map;
        camera_controller_.set_anchor(camera_controller_.anchor() + drag_sensitivity * delta);

        mouse_drag_origin = Input.mousePosition;
    }
}
