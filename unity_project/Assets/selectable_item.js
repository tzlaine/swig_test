#pragma strict

var value : String;
var deselectable : boolean = false;
var toggle_group_ : toggle_group;


function value_changed (b : boolean)
{
    if (b && toggle_group_)
        toggle_group_.set_value(value);
    if (!deselectable)
        GetComponent(UI.Toggle).isOn = true;
}
