#pragma strict

import SimpleJSON;
import System.IO;
import System.Collections.Generic;

var alpha_front : counter_sheet;
var bravo_front : counter_sheet;
var bases_front : counter_sheet;
var bases_back : counter_sheet;
var counters_front : counter_sheet;

private var sheets : counter_sheet[] = new counter_sheet[5];
private var sheet_index : int = 0;
private var json : SimpleJSON.JSONNode = null;
private var nation : String = 'FED';
private var unit : String = 'BATS';
private var nation_index : int = 0;
private var unit_index : int = 0;
private var nations : String[];
private var units : String[];
private var textures : Texture2D[];
private var scroll_position = Vector2(0, 0);
private var has_crippled_side = true;
private var crippled_texture : Texture2D = null;
private var crippled_texture_uvs : Rect = new Rect();

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
    textures[unit_index_] = main_tex;
}

function populate_units ()
{
    var n : SimpleJSON.JSONNode = json[nation];
    units = new String[n.Count];
    textures = new Texture2D[n.Count];
    var i = 0;
    for (var u : System.Collections.Generic.KeyValuePair.<String, JSONNode> in n) {
        units[i] = u.Key;
        if (u.Value['texture'])
            set_unit_texture(i, u.Value);
        ++i;
    }
}

function change_sheet (forward : boolean)
{
    sheets[sheet_index].gameObject.SetActive(false);
    sheet_index += forward ? 1 : sheets.Length - 1;
    sheet_index = sheet_index % sheets.Length;
    sheets[sheet_index].gameObject.SetActive(true);
}

function next_sheet ()
{ change_sheet(true); }

function prev_sheet ()
{ change_sheet(false); }

function Start ()
{
    sheets[0] = alpha_front;
    sheets[1] = bravo_front;
    sheets[2] = bases_front;
    sheets[3] = bases_back;
    sheets[4] = counters_front;

    sheets[sheet_index].gameObject.SetActive(true);

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
        prev_sheet();
    else if (Input.GetKeyUp('e'))
        next_sheet();

    if (Input.GetKeyUp(KeyCode.Delete) || Input.GetKeyUp(KeyCode.Backspace)) {
        json[nation].Remove(unit);
        populate_units();
        unit_index = 0;
        unit = units[unit_index];
        has_crippled_side = true;
        crippled_texture = null;
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
        populate_units();
        unit_index = 0;
        unit = units[unit_index];
        has_crippled_side = true;
        crippled_texture = null;
    }

    var scroll_region_height = units.length * 80;

    scroll_position = GUI.BeginScrollView(
        Rect(70, 10, 180, Screen.height - 20),
        scroll_position,
        Rect(0, 0, 160, scroll_region_height)
    );

    var style = new GUIStyle(GUI.skin.GetStyle('Button'));
    var u_index =
        GUI.SelectionGrid(Rect(10, 10, 140, scroll_region_height), unit_index, units, 1, style);
    if (u_index != unit_index) {
        unit_index = u_index;
        unit = units[unit_index];
        has_crippled_side = true;
        crippled_texture = null;
    }

    GUI.EndScrollView();

    var group_width = 330;
    GUI.BeginGroup(Rect(360, 10, group_width, 30 + 25 + 150), new GUIStyle(GUI.skin.GetStyle('Button')));
    GUI.Label(Rect(10, 10, group_width - 20, 25), unit, new GUIStyle(GUI.skin.GetStyle('Button')));

    var uncrippled_counter = new counter_side_t();
    uncrippled_counter.texture_filename = json[nation][unit]['texture'];
    uncrippled_counter.uv_min = Vector2(
        json[nation][unit]['uv_min']['u'].AsFloat,
        json[nation][unit]['uv_min']['v'].AsFloat
    );
    uncrippled_counter.uv_max = Vector2(
        json[nation][unit]['uv_max']['u'].AsFloat,
        json[nation][unit]['uv_max']['v'].AsFloat
    );

    var uv_size = uncrippled_counter.uv_max - uncrippled_counter.uv_min;
    var uncrippled_uvs : Rect = new Rect(
        uncrippled_counter.uv_min.x,
        uncrippled_counter.uv_min.y,
        uv_size.x,
        uv_size.y
    );
    if (textures[unit_index]) {
        GUI.DrawTextureWithTexCoords(
            Rect(10, 45, 150, 150),
            textures[unit_index],
            uncrippled_uvs
        );
    }

    if (!crippled_texture && has_crippled_side) {
        var crippled_counter_ = game_data.crippled_side(uncrippled_counter);
        crippled_texture = AssetDatabase.LoadAssetAtPath(
            crippled_counter_.texture_filename,
            Texture2D
        );
        if (!crippled_texture) {
            has_crippled_side = false;
        } else {
            uv_size = crippled_counter_.uv_max - crippled_counter_.uv_min;
            crippled_texture_uvs = new Rect(
                crippled_counter_.uv_min.x,
                crippled_counter_.uv_min.y,
                uv_size.x,
                uv_size.y
            );
        }
    }

    if (crippled_texture) {
        GUI.DrawTextureWithTexCoords(
            Rect(170, 45, 150, 150),
            crippled_texture,
            crippled_texture_uvs
        );
    }

    GUI.EndGroup();
}
