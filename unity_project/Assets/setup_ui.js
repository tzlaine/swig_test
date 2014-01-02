#pragma strict

@DoNotSerialize
var game_data_ : game_data;

var nation : String;

function Awake ()
{ enabled = false; }

function OnGUI ()
{
    var setup_panel_width = 300;

    GUILayout.BeginArea(
        Rect(Screen.width - setup_panel_width, 0, setup_panel_width, Screen.height),
        new GUIStyle(GUI.skin.box)
    );

    GUILayout.BeginVertical();

    GUILayout.Label('Setting up ' + game_data_.short_name(nation));

    // TODO
    if (GUILayout.Button('Done')) {
        enabled = false;
    }

    GUILayout.EndVertical();

    GUILayout.EndArea();
}
