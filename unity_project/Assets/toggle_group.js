#pragma strict


class toggle_group extends UI.ToggleGroup
{
    var notify : func.string_delegate;

    private var value_ : String;

    function value () : String
    { return value_; }

    function set_value (v : String)
    {
        if (notify && value_ != v)
            notify(v);
        value_ = v;
    }
}
