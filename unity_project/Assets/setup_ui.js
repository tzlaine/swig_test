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
private var pending_counters : Dictionary.<hex_coord, Array>;

private var nation_ : String;
private var fleet_names : String[];
private var fleet_selection : int = 0;
private var fleet_scroll_position : Vector2 = Vector2(0, 0);

private var unit_names_with_quantities : String[];
private var unit_names : String[];
private var unit_selection : int = 0;
private var unit_scroll_position : Vector2 = Vector2(0, 0);

private var all_units_placed : boolean;


function set_up (n : String)
{
    nation_ = n;

    fleet_selection = 0;
    unit_selection = 0;

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

    place_hexes_.highlight_hexes(nation_, fleet_names[fleet_selection]);

    set_units(false);

    enabled = true;
}

function set_units (keep_selection_and_scroll : boolean)
{
    var fleet =
        game_data_.nation(nation_).starting_forces[fleet_names[fleet_selection]].units.units;
    unit_names_with_quantities = new String[fleet.Count];
    unit_names = new String[fleet.Count];
    var i = 0;
    for (unit_type in fleet) {
        unit_names[i] = unit_type.Key;
        unit_names_with_quantities[i] =
            (1 < unit_type.Value.first ? unit_type.Value.first + 'x' : '') + unit_type.Key;
        ++i;
    }

    if (keep_selection_and_scroll) {
        if (fleet.Count <= unit_selection)
            unit_selection = Mathf.Max(0, fleet.Count - 1);
    } else {
        unit_selection = 0;
        unit_scroll_position = Vector2(0, 0);
    }
}

function Awake ()
{
    if (!JSONLevelSerializer.IsDeserializing)
        enabled = false;
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

private function select_fleet (fleet_index)
{
    fleet_selection = fleet_index;

    set_units(false);
    place_hexes_.clear_highlighting();
    place_hexes_.highlight_hexes(nation_, fleet_names[fleet_selection]);

    var starting_forces = game_data_.nation(nation_).starting_forces;
    var fleet = starting_forces[fleet_names[fleet_selection]];
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

function hex_clicked (hc : hex_coord)
{
    if (fleet_selection < fleet_names.Length &&
        unit_selection < unit_names.Length) {
        var fleet =
            game_data_.nation(nation_).starting_forces[fleet_names[fleet_selection]];
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
                   fleet_selection < fleet_names.Length &&
                   unit_selection < unit_names.Length) {
                place(unit_names[unit_selection], fleet, hex, limit);
            }
        } else {
            place(unit_names[unit_selection], fleet, hex, limit);
        }
        game_state_.save_async();

        if (!fleet.units.size() && fleet_selection < fleet_names.Length - 1)
            select_fleet(fleet_selection + 1);
    }
}

function OnEnable ()
{
    if (!JSONLevelSerializer.IsDeserializing)
        place_hexes_.highlight_hexes(nation_, fleet_names[fleet_selection]);
    map_click_listener_.hex_clicked_callback = hex_clicked;
}

function OnGUI ()
{
    if (!game_data_.map())
        return;

    if (pending_counters && place_hexes_.ready()) {
        for (hc in pending_counters) {
            var hex_center = place_hexes_.hex_center(hc.Key);
            for (c in hc.Value) {
                var go = c as GameObject;
                go.transform.position = hex_center;
                go.SetActive(true);
            }
        }
        pending_counters = null;
    }

    if (!unit_names_with_quantities)
        set_units(false);

    var right_panel_width = 200;
    var bottom_panel_height = 50;

    // Right side-panel.
    GUILayout.BeginArea(
        Rect(Screen.width - right_panel_width, 0, right_panel_width, Screen.height),
        new GUIStyle(GUI.skin.box)
    );

    GUILayout.BeginVertical();

    GUILayout.Label('Setting up ' + game_data_.short_name(nation_) + ' fleets');

    if (unit_names.Length) {
        var counter = game_data_.counter(nation_, unit_names[unit_selection]);
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
            GUI.DrawTextureWithTexCoords(
                Rect(25, 30, 150, 150),
                counter_side.texture,
                uncrippled_uvs
            );
        }
    }

    GUILayout.Space(160);

    unit_scroll_position = GUILayout.BeginScrollView(unit_scroll_position);
    unit_selection = GUILayout.SelectionGrid(unit_selection, unit_names_with_quantities, 1);
    GUILayout.EndScrollView();

    GUI.enabled = all_units_placed;
    if (GUILayout.Button('Done')) {
        place_hexes_.clear_highlighting();
        enabled = false;
    }
    GUI.enabled = true;

    GUILayout.EndVertical();

    GUILayout.EndArea();

    // Bottom panel.
    GUILayout.BeginArea(
        Rect(0,
             Screen.height - bottom_panel_height,
             Screen.width - right_panel_width,
             bottom_panel_height),
        new GUIStyle(GUI.skin.box)
    );

    fleet_scroll_position = GUILayout.BeginScrollView(fleet_scroll_position);
    var new_fleet_selection =
        GUILayout.SelectionGrid(fleet_selection, fleet_names, fleet_names.Length);
    if (new_fleet_selection != fleet_selection)
        select_fleet(new_fleet_selection);
    GUILayout.EndScrollView();

    GUILayout.EndArea();
}
