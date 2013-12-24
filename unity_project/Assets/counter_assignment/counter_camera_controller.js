var mouse_sensitivity : float = 15.0;
var key_zoom_sensitivity : float = 1.0;
var mousewheel_zoom_sensitivity : float = 1.0;
var move_speed : float = 16.0;
var min_distance : float = 5;
var max_distance : float = 150;

private var anchor_ = Vector3.zero;
private var distance : float = 100.0;
private var rotation_x : float = 0.0;
private var rotation_y : float = 0.0;
private var max_rot : float = 70.0;
private var dirty = true;


function anchor () : Vector3
{ return anchor_; }

function set_anchor (a : Vector3)
{
    anchor_ = clamp_anchor(a);
    dirty = true;
}

function clamp_anchor (a : Vector3) : Vector3
{
    var width = 40;
    var height = 40;
    a.x = Mathf.Clamp(a.x, -width / 2, width / 2);
    a.y = Mathf.Clamp(a.y, -height / 2, height / 2);
    return a;
}

function position_camera ()
{
    transform.position = anchor_;
    transform.localRotation = Quaternion.AngleAxis(rotation_x, Vector3.forward);
    transform.localRotation *= Quaternion.AngleAxis(rotation_y, Vector3.left);
    transform.position += distance * transform.TransformDirection(-Vector3.forward);
}

function Start ()
{ position_camera(); }

function Update ()
{
    var zoom_input : float =
        Input.GetAxis('Mouse ScrollWheel') * mousewheel_zoom_sensitivity;
    if (zoom_input != 0) {
        distance = Mathf.Clamp(distance + zoom_input, min_distance, max_distance);
        dirty = true;
    }

    var world_up = transform.TransformDirection(Vector3.up);
    var world_left = transform.TransformDirection(Vector3.left);

    var up_projected_on_map = Vector3.Cross(world_left, Vector3.forward);
    var left_projected_on_map = Vector3.Cross(-Vector3.forward, world_up);

    var move_direction =
        left_projected_on_map * Input.GetAxis('Horizontal') +
        up_projected_on_map * Input.GetAxis('Vertical');
    if (move_direction.sqrMagnitude) {
        move_direction = move_speed * move_direction;
        anchor_ += move_direction * Time.deltaTime;
        anchor_ = clamp_anchor(anchor_);
        dirty = true;
    }

    if (dirty) {
        position_camera();
        dirty = false;
    }
}
