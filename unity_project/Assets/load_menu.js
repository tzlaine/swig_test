#pragma strict

import System.IO;

var start_menu_ : GameObject;
var list : listbox;
var list_element : GameObject;

private var game_filenames : String[];
private var game_names : String[];
private var list_elements : RectTransform[];


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
{
    find_saves();

    list_elements = new RectTransform[game_filenames.Length];

    for (var i = 0; i < game_filenames.Length; ++i) {
        var instance : GameObject = Instantiate(list_element);
        var rt : RectTransform = instance.GetComponent(RectTransform);
        var text : UI.Text = instance.GetComponent(UI.Text);
        text.text = game_names[i];
        list_elements[i] = rt;
    }

    list.set_data(list_elements);
}

/*
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
        start_menu_.SetActive(true);
        enabled = false;
    }
    GUILayout.EndHorizontal();

    GUILayout.EndVertical();

    GUILayout.EndHorizontal();

    gui_utils.centered_menu_footer();
}
*/
