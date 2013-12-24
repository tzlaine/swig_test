#pragma strict

private var file : String;
private var assign_counters_ : assign_counters = null;
private var collider_ : MeshCollider = null;

function Awake ()
{
    collider_ = GetComponent(MeshCollider);
    assign_counters_ = FindObjectOfType(assign_counters);
}

function init (file_ : String)
{
    file = 'Assets/counters/' + file_ + '.png';
    renderer.material.mainTexture = AssetDatabase.LoadAssetAtPath(file, Texture2D);
}

function counter_index (click_x : float)
{
    var distance = Mathf.Abs(click_x - transform.position.x);
    var direction = transform.position.x < click_x ? 1 : -1;
    if (distance < 1.5 / 2.0)
        return 2;
    else
        return 2 + direction * Mathf.FloorToInt((distance + 1.5 / 2.0) / 1.5);
}

function OnMouseUpAsButton ()
{
    var hit : RaycastHit;
    if (collider_.Raycast(Camera.main.ScreenPointToRay(Input.mousePosition), hit, 1000.0)) {
        var i = counter_index(hit.point.x);
        var uv_min = Vector2(i * 0.2, 0.0);
        var uv_max = Vector2((i + 1) * 0.2, 1.0);
        assign_counters_.assign(file, uv_min, uv_max);
    }
}
