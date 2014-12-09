#pragma strict

@script SerializeAll

@DoNotSerialize
var game_data_ : game_data;
@DoNotSerialize
var game_state_ : game_state;
@DoNotSerialize
var place_hexes_ : place_hexes;
@DoNotSerialize
var map_click_listener_ : map_click_listener;
@DoNotSerialize
var counter_ : counter;
@DoNotSerialize
var button_prefab : GameObject;

@DoNotSerialize
var title : UI.Text;
@DoNotSerialize
var counter_image : UI.Image;
@DoNotSerialize
var units_list : listbox;
@DoNotSerialize
var fleets_list : listbox;
@DoNotSerialize
var units_group : toggle_group;
@DoNotSerialize
var fleets_group : toggle_group;
@DoNotSerialize
var done_button : UI.Button;

var done : boolean = false;

@DoNotSerialize
private var pending_counters : Dictionary.<hex_coord, Array>;

private var nation_ : String;
private var fleet_buttons : GameObject[];
private var fleet_names : String[];

private var unit_buttons : GameObject[];
private var unit_names : String[];

private var all_units_placed : boolean;


private function current_unit () : int
{
    var retval : int = -1;
    try {
        retval = parseInt(units_group.value());
    } catch (e) {}
    return retval;
}

private function current_fleet () : String
{ return fleets_group.value(); }

function Awake ()
{
    units_group.notify = select_unit;
    fleets_group.notify = select_fleet;
    done_button.interactable = false;
}

function OnEnable ()
{
    if (!JSONLevelSerializer.IsDeserializing)
        place_hexes_.highlight_hexes(nation_, current_fleet());
    map_click_listener_.hex_clicked_callback = hex_clicked;
}

function done_clicked ()
{
    place_hexes_.clear_highlighting();
    done = true;
    gameObject.SetActive(false);
}

function set_up (n : String)
{
    nation_ = n;

    pending_counters = new Dictionary.<hex_coord, Array>();

    all_units_placed = true;

    var starting_forces = game_data_.nation(nation_).starting_forces;

    var names = new Array();
    for (f in starting_forces) {
        if (f.Value.area.Length == 1) {
            var hex = game_data_.map().hex(f.Value.area[0]);
            var fleet = f.Value.units.units;
            for (unit_type in fleet) {
                var i = 0;
                var placed_counter : counter;
                for (i = 0; i < unit_type.Value.first; ++i) {
                    hex.insert(nation_, unit_type.Key, false);
                    placed_counter = Instantiate(counter_);
                    placed_counter.init(nation_, unit_type.Key, false);
                    placed_counter.gameObject.SetActive(false);
                    if (!pending_counters.ContainsKey(hex.hc))
                        pending_counters.Add(hex.hc, new Array());
                    pending_counters[hex.hc].Push(placed_counter.gameObject);
                }
                for (i = 0; i < unit_type.Value.second; ++i) {
                    hex.insert(nation_, unit_type.Key, true);
                    placed_counter = Instantiate(counter_);
                    placed_counter.init(nation_, unit_type.Key, true);
                    placed_counter.gameObject.SetActive(false);
                    if (!pending_counters.ContainsKey(hex.hc))
                        pending_counters.Add(hex.hc, new Array());
                    pending_counters[hex.hc].Push(placed_counter.gameObject);
                }
            }
            f.Value.units.clear();
        } else {
            names.Push(f.Key);
            if (f.Value.units.size())
                all_units_placed = false;
        }
    }

    fleet_names = names.ToBuiltin(String);
    fleet_buttons = new GameObject[fleet_names.Length];

    i = 0;
    for (var fleet_name : String in fleet_names) {
        var instance : GameObject = Instantiate(button_prefab);
        fleet_buttons[i] = instance;

        var text : UI.Text = instance.GetComponentInChildren(UI.Text);
        text.text = fleet_name;

        var selectable_item_ : selectable_item =
            instance.GetComponent(selectable_item);
        selectable_item_.value = fleet_name;
        selectable_item_.toggle_group_ = fleets_group;

        var toggle : UI.Toggle = instance.GetComponentInChildren(UI.Toggle);
        toggle.group = fleets_group;
        if (!i)
            toggle.isOn = true;

        ++i;
    }
    fleets_list.set_data(fleet_buttons, 125);

    select_fleet(fleet_names[0]);

    done_button.interactable = false;
    done = false;

    if (!gameObject.active)
        gameObject.SetActive(true);
}

private function set_image ()
{
    if (!unit_names.Length)
        return;

    var counter = game_data_.counter(nation_, unit_names[current_unit()]);
    var counter_side = counter.uncrippled_side;
    if (!counter_side.texture) {
        counter_side.texture =
            AssetDatabase.LoadAssetAtPath(counter_side.texture_filename, Texture2D);
    }
    if (counter_side.texture) {
        var uv_size = counter_side.uv_max - counter_side.uv_min;
        var uncrippled_uvs : Rect = new Rect(
            counter_side.uv_min.x,
            counter_side.uv_min.y,
            uv_size.x,
            uv_size.y
        );
//        GUI.DrawTextureWithTexCoords(
//            Rect(25, 30, 150, 150),
//            counter_side.texture,
//            uncrippled_uvs
//        );
    }
}

private function select_fleet (fleet_name : String)
{
    title.text = fleet_name;

    set_units(false);
    place_hexes_.clear_highlighting();
    place_hexes_.highlight_hexes(nation_, fleet_name);

    var starting_forces = game_data_.nation(nation_).starting_forces;
    var fleet = starting_forces[fleet_name];
    for (var i = 0; i < fleet.area.Length; ++i) {
        if (fleet.area[i] == hex_coord())
            continue;
        var hex = game_data_.map().hex(fleet.area[i]);
        if (fleet.area_unit_limits[i] &&
            hex.units.ContainsKey(nation_) &&
            fleet.area_unit_limits[i] <= hex.units[nation_].size()) {
            var hexes = new hex_coord[1];
            hexes[0] = fleet.area[i];
            place_hexes_.unhighlight_hexes(hexes);
        }
    }

    game_state_.save_async();
}

private function select_unit (unit_name : String)
{
Debug.Log('select_unit(' + unit_name + ')');
    // TODO
}

private function set_units (keep_selection_and_scroll : boolean)
{
    var fleet =
        game_data_.nation(nation_).starting_forces[current_fleet()].units.units;
    unit_buttons = new GameObject[fleet.Count];
    unit_names = new String[fleet.Count];
    var i = 0;
    for (unit_type in fleet) {
        unit_names[i] = unit_type.Key;

        var button_text : String =
            (1 < unit_type.Value.first ? unit_type.Value.first + 'x' : '') + unit_type.Key;

        var instance : GameObject = Instantiate(button_prefab);
        unit_buttons[i] = instance;

        var text : UI.Text = instance.GetComponentInChildren(UI.Text);
        text.text = button_text;

        var selectable_item_ : selectable_item =
            instance.GetComponent(selectable_item);
        selectable_item_.value = i.ToString();
        selectable_item_.toggle_group_ = units_group;

        var toggle : UI.Toggle = instance.GetComponentInChildren(UI.Toggle);
        toggle.group = units_group;

        ++i;
    }

    var unit : int = current_unit();
    if (keep_selection_and_scroll)
        unit = current_unit();
    units_list.set_data(unit_buttons, 25);
    if (unit_buttons.Length)
        unit_buttons[unit == -1 ? 0 : unit].GetComponent(UI.Toggle).isOn = true;
}

private function hex_clicked (hc : hex_coord)
{
    Debug.Log('hex_clicked()');

    if (current_unit() < 0 || unit_names.Length <= current_unit())
        return;

    var fleet = game_data_.nation(nation_).starting_forces[current_fleet()];
    var hex : hex_t;
    var limit = 10000;
    var in_fleet_area = false;
    var i = 0;
    for (hc2 in fleet.area) {
        if (hc2 == hc) {
            hex = game_data_.map().hex(hc);
            if (hex.highlight) {
                if (fleet.area_unit_limits[i])
                    limit = fleet.area_unit_limits[i];
                in_fleet_area = true;
                break;
            }
        }
        ++i;
    }

    if (!in_fleet_area)
        return;

    if (Input.GetKey(KeyCode.RightAlt) || Input.GetKey(KeyCode.LeftAlt)) {
        while (hex.highlight &&
               0 <= current_unit() && current_unit() < unit_names.Length) {
            place(unit_names[current_unit()], fleet, hex, limit);
        }
    } else {
        place(unit_names[current_unit()], fleet, hex, limit);
    }
    game_state_.save_async();

    var fleet_selection : int = -2;
    for (i = 0; i < fleet_names.Length; ++i) {
        if (current_fleet() == fleet_names[i])
            fleet_selection = i;
    }
    if (!fleet.units.size() && fleet_selection < fleet_names.Length - 1) {
        var toggle : UI.Toggle =
            fleet_buttons[fleet_selection + 1].GetComponentInChildren(UI.Toggle);
        toggle.isOn = true;
    }

    if (all_units_placed)
        done_button.interactable = true;
}

private function place (unit : String,
                        fleet : starting_fleet,
                        hex : hex_t,
                        limit : int)
{
    var hex_center = place_hexes_.hex_center(hex.hc);
    var placed_counter : counter =
        Instantiate(counter_, hex_center, Quaternion.identity);
    placed_counter.init(nation_, unit, false);

    hex.insert(nation_, unit, false);
    fleet.units.erase(unit, false);
    set_units(true);

    if (!fleet.units.size()) {
        all_units_placed = true;
        for (f in game_data_.nation(nation_).starting_forces) {
            if (f.Value.units.size())
                all_units_placed = false;
        }
    }

    var units_in_hex = hex.units[nation_].size();
    if (limit == units_in_hex) {
        var hexes = new hex_coord[1];
        hexes[0] = hex.hc;
        place_hexes_.unhighlight_hexes(hexes);
    }
}
