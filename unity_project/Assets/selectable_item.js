#pragma strict

var value : String;
var toggle_group_ : toggle_group;


function value_changed (b : boolean)
{
    if (b)
        toggle_group_.set_value(value);
}
