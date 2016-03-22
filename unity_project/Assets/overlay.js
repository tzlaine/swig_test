#pragma strict

class overlay_data
{
    function color (i : int) : Vector4
    { return !colors_ ? color_ : colors_[i]; }

    var color_ : Vector4;
    var colors_ : Vector4[];
    var hexes_ : hex_coord[];
};

class overlay_set
{
    function overlay_set ()
    { by_key_ = new Dictionary.<String, overlay_data>(); }

    var by_key_ : Dictionary.<String, overlay_data>;
};

private var active_overlay : String = '';
private var overlays : Dictionary.<String, overlay_set> =
    new Dictionary.<String, overlay_set>();

function add_overlay_set (name : String, set_ : overlay_set)
{ overlays[name] = set_; }

function activate_overlay (name : String)
{
    active_overlay = name;

    // TODO: Show this overlay...
}
