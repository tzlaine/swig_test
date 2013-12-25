#pragma strict

import SimpleJSON;
import System.IO;
import System.Collections.Generic;

var counter_sheet_column_ : counter_sheet_column;


private var columns : counter_sheet_column[] = new counter_sheet_column[6];
private var column_index : int = 0;
private var json : SimpleJSON.JSONNode = null;
private var nation : String = 'FED';
private var unit : String = 'DN';
private var nation_index : int = 0;
private var unit_index : int = 0;
private var nations : String[];
private var units : GUIContent[];
private var scroll_position = Vector2(0, 0);

function save ()
{ System.IO.File.WriteAllText('../unit_counters.json', json.ToString()); }

function assign (texture : String, uv_min : Vector2, uv_max : Vector2)
{
    json[nation][unit]['texture'] = texture;
    json[nation][unit]['uv_min']['u'].AsFloat = uv_min.x;
    json[nation][unit]['uv_min']['v'].AsFloat = uv_min.y;
    json[nation][unit]['uv_max']['u'].AsFloat = uv_max.x;
    json[nation][unit]['uv_max']['v'].AsFloat = uv_max.y;
    set_unit_texture(unit_index, json[nation][unit]);
    save();
}

function set_unit_texture (unit_index_ : int, unit_json : SimpleJSON.JSONNode)
{
    var main_tex : Texture2D =
        AssetDatabase.LoadAssetAtPath(unit_json['texture'], Texture2D);
    units[unit_index_].image = main_tex;

/*
    var main_tex_width = 1024;
    var main_tex_height = 256;
    var sub_tex_width = main_tex_width;
    var sub_tex_x = Mathf.FloorToInt(unit_json['uv_min']['x'].AsFloat * main_tex_width);
    var sub_tex_y = Mathf.FloorToInt(unit_json['uv_min']['y'].AsFloat * main_tex_height);

    var main_tex_pixels : Color32[] = main_tex.GetPixels32(); // TODO: Why is this throwing?
    var tex_pixels : Color32[] = new Color32[sub_tex_width * sub_tex_width];
    var i = 0;
    for (var j = 0; j < sub_tex_width; ++j) {
        var base = j * main_tex_width;
        for (var k = 0; k < sub_tex_width; ++k) {
            tex_pixels[i++] = main_tex_pixels[base + k];
        }
    }
    var tex = new Texture2D(sub_tex_width, sub_tex_width, main_tex.format, false);
    tex.SetPixels32(tex_pixels);
    tex.Apply();
    //units[unit_index_].image = tex;
/**/
}

function populate_units ()
{
    var n : SimpleJSON.JSONNode = json[nation];
    units = new GUIContent[n.Count];
    var i = 0;
    for (var u : System.Collections.Generic.KeyValuePair.<String, JSONNode> in n) {
        units[i] = new GUIContent(u.Key);
        if (u.Value['texture'])
            set_unit_texture(i, u.Value);
        ++i;
    }
}

function populate (from : String, col : String)
{
    var retval : counter_sheet_column = Instantiate(counter_sheet_column_);
    var files : String[] = [
        from + '_0' + col,
        from + '_1' + col,
        from + '_2' + col,
        from + '_3' + col,
        from + '_4' + col,
        from + '_5' + col,
        from + '_6' + col
    ];
    retval.init(files);
    retval.gameObject.SetActive(false);
    return retval;
}

function change_column (forward : boolean)
{
    columns[column_index].gameObject.SetActive(false);
    column_index += forward ? 1 : columns.Length - 1;
    column_index = column_index % columns.Length;
    columns[column_index].gameObject.SetActive(true);
}

function next_column ()
{ change_column(true); }

function prev_column ()
{ change_column(false); }

function Start ()
{
    columns[0] = populate('alpha_front_cut', '_0');
    columns[1] = populate('alpha_front_cut', '_1');
    columns[2] = populate('bravo_front_cut', '_0');
    columns[3] = populate('bravo_front_cut', '_1');
    columns[4] = populate('bases_front_cut', '');
    columns[5] = populate('bases_back_cut', '');

    columns[column_index].gameObject.SetActive(true);

    var units_json : SimpleJSON.JSONNode =
        JSON.Parse(System.IO.File.ReadAllText('../units.json'));

    if (System.IO.File.Exists('../unit_counters.json'))
        json = JSON.Parse(System.IO.File.ReadAllText('../unit_counters.json'));
    else
        json = new SimpleJSON.JSONClass();

    var n_index = 0;
    var u_index = 0;
    var nations_ = new Array();
    for (var n : System.Collections.Generic.KeyValuePair.<String, JSONNode> in units_json) {
        nations_.Push(n.Key);
        for (var u : System.Collections.Generic.KeyValuePair.<String, JSONNode> in n.Value) {
            if (json[n.Key][u.Key]['texture'] == null) {
                json[n.Key][u.Key]['texture'] = '';
                json[n.Key][u.Key]['uv_min']['u'].AsFloat = 0;
                json[n.Key][u.Key]['uv_min']['v'].AsFloat = 0;
                json[n.Key][u.Key]['uv_max']['u'].AsFloat = 1;
                json[n.Key][u.Key]['uv_max']['v'].AsFloat = 1;
                if (unit == null) {
                    nation = n.Key;
                    unit = u.Key;
                    nation_index = n_index;
                    unit_index = u_index;
                }
            }
            ++u_index;
        }
        ++n_index;
    }

    var i = 0;

    nations = new String[nations_.length];
    for (i = 0; i < nations.Length; ++i) {
        nations[i] = nations_[i];
    }

    populate_units();
}

function Update ()
{
    if (Input.GetKeyUp('q'))
        prev_column();
    else if (Input.GetKeyUp('e'))
        next_column();

    if (Input.GetKeyUp(KeyCode.Delete) || Input.GetKeyUp(KeyCode.Backspace)) {
        json[nation].Remove(unit);
        unit_index = 0;
        unit = units[unit_index].text;
        populate_units();
        save();
    }
}

function OnGUI ()
{
    var n_index =
        GUI.SelectionGrid(Rect(10, 10, 50, Screen.height - 20), nation_index, nations, 1);
    if (n_index != nation_index) {
        nation_index = n_index;
        nation = nations[nation_index];
        unit_index = 0;
        unit = units[unit_index].text;
        populate_units();
    }

    var scroll_region_height = units.length * 80;

    scroll_position = GUI.BeginScrollView(
        Rect(70, 10, 280, Screen.height - 20),
        scroll_position,
        Rect(0, 0, 260, scroll_region_height)
    );

    var style = new GUIStyle(GUI.skin.GetStyle('Button'));
    style.imagePosition = ImagePosition.ImageAbove;
    var u_index =
        GUI.SelectionGrid(Rect(10, 10, 240, scroll_region_height), unit_index, units, 1, style);
    if (u_index != unit_index) {
        unit_index = u_index;
        unit = units[unit_index].text;
    }
    
    GUI.EndScrollView();
}
