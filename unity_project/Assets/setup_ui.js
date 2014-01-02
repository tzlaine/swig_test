#pragma strict

@script SerializeAll

@DoNotSerialize
var game_data_ : game_data;
@DoNotSerialize
var game_state_ : game_state;
@DoNotSerialize
var place_hexes_ : place_hexes;

private var nation_ : String;
private var fleet_names : String[];
private var fleet_selection : int = 0;
private var scroll_position : Vector2 = Vector2(0, 0);

function set_up (n : String)
{
    nation_ = n;

    fleet_selection = 0;

    var starting_forces = game_data_.nation(nation_).starting_forces;

    fleet_names = new String[starting_forces.Count];

    var i = 0;
    for (var f : System.Collections.Generic.KeyValuePair.<String, starting_fleet> in
         starting_forces) {
        fleet_names[i] = f.Key;
        ++i;
    }

    place_hexes_.highlight_hexes(nation_, fleet_names[fleet_selection]);

    enabled = true;
}

function Awake ()
{
    if (!JSONLevelSerializer.IsDeserializing)
        enabled = false;
}

function OnEnable ()
{
    if (!JSONLevelSerializer.IsDeserializing)
        place_hexes_.highlight_hexes(nation_, fleet_names[fleet_selection]);
}

function OnGUI ()
{
    if (!game_data_.map())
        return;

    var right_panel_width = 300;
    var bottom_panel_height = 50;

    // Right side-panel.
    GUILayout.BeginArea(
        Rect(Screen.width - right_panel_width, 0, right_panel_width, Screen.height),
        new GUIStyle(GUI.skin.box)
    );

    GUILayout.BeginVertical();

    GUILayout.Label('Setting up ' + game_data_.short_name(nation_));

    // TODO
    if (GUILayout.Button('Done')) {
        place_hexes_.clear_highlighting();
        enabled = false;
    }

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

    scroll_position = GUILayout.BeginScrollView(scroll_position);
    var new_fleet_selection =
        GUILayout.SelectionGrid(fleet_selection, fleet_names, fleet_names.Length);
    if (new_fleet_selection != fleet_selection) {
        fleet_selection = new_fleet_selection;
        place_hexes_.clear_highlighting();
        place_hexes_.highlight_hexes(nation_, fleet_names[fleet_selection]);
        game_state_.save_async();
    }
    GUILayout.EndScrollView();

    GUILayout.EndArea();
}
