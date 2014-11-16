#pragma strict

import SimpleJSON;
import System.IO;
import gui_utils;

var start_menu_ : GameObject;

private var scenarios : SimpleJSON.JSONNode[];
private var scenario_names : String[];
private var scenario_descriptions : String[];
private var selection = 0;


function scenario () : SimpleJSON.JSONNode
{ return scenarios[selection]; }

function Awake ()
{
    var data = new Array();

    var info = new DirectoryInfo('../scenarios');
    var files = info.GetFiles();

    scenarios = new SimpleJSON.JSONNode[files.Length];
    scenario_names = new String[files.Length];
    scenario_descriptions = new String[files.Length];

    for (var i = 0; i < files.Length; ++i) {
        scenarios[i] =
            JSON.Parse(System.IO.File.ReadAllText(files[i].ToString()));
        var name : String = scenarios[i]['name'];
        scenario_names[i] = name;
        var desc : String = scenarios[i]['description'];
        scenario_descriptions[i] = desc.Replace('\\n', '\n');
    }

    enabled = false;
}

function OnGUI ()
{
    gui_utils.centered_menu_header();

    GUILayout.BeginHorizontal(new GUIStyle(GUI.skin.box));

    GUILayout.BeginVertical();

    var title_style = new GUIStyle(GUI.skin.label);
    title_style.alignment = TextAnchor.MiddleCenter;
    GUILayout.Label('Select a scenario', title_style);

    GUILayout.BeginHorizontal();
    selection = GUILayout.SelectionGrid(selection, scenario_names, 1, GUILayout.MinWidth(200));
    GUILayout.Label(scenario_descriptions[selection], GUILayout.MaxWidth(300));
    GUILayout.EndHorizontal();

    GUILayout.BeginHorizontal();
    if (GUILayout.Button('Next')) {
        GetComponent(game_setup).set_up(scenario());
        enabled = false;
    }
    if (GUILayout.Button('Cancel')) {
        start_menu_.SetActive(true);
        enabled = false;
    }
    GUILayout.EndHorizontal();

    GUILayout.EndVertical();

    GUILayout.EndHorizontal();

    gui_utils.centered_menu_footer();
}
