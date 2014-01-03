#pragma strict

var assign_counters_ : assign_counters = null;
var file_base : String;

private var collider_ : MeshCollider = null;
private var file : String;

function Awake ()
{
    file = 'Assets/counters/' + file_base + '_cut.png';
    collider_ = GetComponent(MeshCollider);
}

function OnMouseUpAsButton ()
{
    var scale = transform.localScale;
    var counter_width = 1.5;
    var counter_uv = Vector2(counter_width / scale.x, counter_width / scale.y);
    var sheet_origin =
        -Vector3(scale.x, scale.y, 0) / 2.0;
    var hit : RaycastHit;
    if (collider_.Raycast(Camera.main.ScreenPointToRay(Input.mousePosition), hit, 1000.0)) {
        var delta = hit.point - sheet_origin;
        var uv_min = Vector2(
            Mathf.FloorToInt(delta.x / counter_width) * counter_width / scale.x,
            Mathf.FloorToInt(delta.y / counter_width) * counter_width / scale.y
        );
        var uv_max = uv_min + counter_uv;
        assign_counters_.assign(file, uv_min, uv_max);
    }
}
