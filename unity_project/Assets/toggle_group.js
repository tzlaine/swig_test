#pragma strict


class toggle_group extends UI.ToggleGroup
{
    private var value_ : String;

    function value () : String
    { return value_; }

    function set_value (v : String)
    { value_ = v; }
}
