#pragma strict


class toggle_group extends UI.ToggleGroup
{
    var notify : func.string_delegate;

    private var value_ : String;

    function value () : String
    { return value_; }

    function set_value (v : String)
    {
        var n : boolean = notify != null && value_ != v;
        value_ = v;
        if (n)
            notify(value_);
    }
}
