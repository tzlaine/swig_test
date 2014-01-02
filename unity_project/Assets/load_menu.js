#pragma strict

import System.IO;
import gui_utils;

var start_menu_ : start_menu;

private var game_filenames : String[];
private var game_names : String[];
private var selection : int = 0;
private var scroll_position : Vector2 = Vector2(0, 0);


private static function file_info_cmp (a : FileInfo, b : FileInfo) : int
{
    if (a.LastWriteTime < b.LastWriteTime)
        return 1;
    if (b.LastWriteTime < a.LastWriteTime)
        return -1;
    return 0;
}

function find_saves ()
{
    var info = new DirectoryInfo(Application.persistentDataPath);
    var files : FileInfo[] = info.GetFiles();

    System.Array.Sort(files, file_info_cmp);

    game_filenames = new String[files.Length];
    game_names = new String[files.Length];

    for (var i = 0; i < files.Length; ++i) {
        var prefix = 36; // length of guid
        game_filenames[i] = files[i].Name;
        game_names[i] =
            game_filenames[i].Substring(prefix, game_filenames[i].Length - prefix) +
            '\n' + files[i].LastWriteTime.ToString();
    }
}

function OnEnable ()
{ find_saves(); }

function Awake ()
{ enabled = false; }

function OnGUI ()
{
    gui_utils.centered_menu_header();

    GUILayout.BeginHorizontal(new GUIStyle(GUI.skin.box));

    GUILayout.BeginVertical();

    var title_style = new GUIStyle(GUI.skin.label);
    title_style.alignment = TextAnchor.MiddleCenter;
    GUILayout.Label('Select a game to load', title_style);

    scroll_position = GUILayout.BeginScrollView(scroll_position);
    selection = GUILayout.SelectionGrid(selection, game_names, 1, GUILayout.MinWidth(300));
    GUILayout.EndScrollView();

    // TODO: Allow deletion of old saves.

    GUILayout.BeginHorizontal();
    if (GUILayout.Button('Load')) {
        JSONLevelSerializer.LoadSavedLevelFromFile(game_filenames[selection]);
        enabled = false;
    }
    if (GUILayout.Button('Cancel')) {
        start_menu_.enabled = true;
        enabled = false;
    }
    GUILayout.EndHorizontal();

    GUILayout.EndVertical();

    GUILayout.EndHorizontal();

    gui_utils.centered_menu_footer();
}
