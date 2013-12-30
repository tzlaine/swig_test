#pragma strict

import SimpleJSON;
import gui_utils;

var game_data_ : game_data;
var start_menu_ : start_menu;
var game_state_ : game_state;

private var scenario : SimpleJSON.JSONNode;
private var teams : Dictionary.<String, List.<String> > =
    new Dictionary.<String, List.<String> >();
private var selection_names : String[];
private var selection_descriptions : String[];
private var selection_configs : Dictionary.<String, boolean>[];
private var selection : int = 0;

private function team_members (team : String)
{
    var retval = '';
    var members = teams[team];
    for (var i = 0; i < members.Count; ++i) {
        if (i)
            retval += ', ';
        retval += game_data_.full_name(members[i]);
    }
    return retval;
}

private function all_false_config ()
{
    var retval = new Dictionary.<String, boolean>();
    for (var team : System.Collections.Generic.KeyValuePair.<String, List.<String> > in
         teams) {
        for (member in team.Value) {
            retval.Add(member, false);
        }
    }
    return retval;
}

function set_up (scenario_ : SimpleJSON.JSONNode)
{
    scenario = scenario_;
    selection = 0;

    for (var team : System.Collections.Generic.KeyValuePair.<String, JSONNode> in
         scenario['teams']) {
        ++selection;
        var team_members = new List.<String>();
        for (var i = 0; i < team.Value.Count; ++i) {
            team_members.Add(team.Value[i]);
            ++selection;
        }
        teams.Add(team.Key, team_members);
    }

    selection_names = new String[selection + 1];
    selection_descriptions = new String[selection + 1];
    selection_configs = new Dictionary.<String, boolean>[selection + 1];

    var arrays_index = 0;
    selection_configs[arrays_index] = new Dictionary.<String, boolean>();

    for (var team_ : System.Collections.Generic.KeyValuePair.<String, List.<String> > in
         teams) {
        selection_names[arrays_index] = team_.Key;
        selection_descriptions[arrays_index] =
            String.Format('Play {0} ({1}); computer plays ', team_.Key, team_members(team_.Key));
        for (member in team_.Value) {
            selection_configs[arrays_index].Add(member, true);
        }
        var first_other_team = true;
        for (var other_team : System.Collections.Generic.KeyValuePair.<String, List.<String> > in
             teams) {
            if (other_team.Key == team_.Key)
                continue;
            if (!first_other_team)
                selection_descriptions[arrays_index] += ', ';
            selection_descriptions[arrays_index] +=
                String.Format('{0} ({1})', other_team.Key, team_members(other_team.Key));
            for (member in other_team.Value) {
                selection_configs[arrays_index].Add(member, false);
            }
            first_other_team = false;
        }
        selection_descriptions[arrays_index] += '.';
        ++arrays_index;

        for (member in team_.Value) {
            selection_names[arrays_index] = game_data_.full_name(member);
            selection_descriptions[arrays_index] = String.Format(
                'Play {0}; computer plays all other allied and enemy nations.',
                game_data_.full_name(member)
            );
            selection_configs[arrays_index] = all_false_config();
            selection_configs[arrays_index][member] = true;
            ++arrays_index;
            selection_configs[arrays_index] = new Dictionary.<String, boolean>();
        }
    }

    selection_names[selection] = 'Hotseat';
    selection_descriptions[selection] = 'Multiple players take turns on the same computer.';
    for (var team__ : System.Collections.Generic.KeyValuePair.<String, List.<String> > in
         teams) {
        for (var l = 0; l < team__.Value.Count; ++l) {
            selection_configs[selection][team__.Value[l]] = true;
        }
    }

    enabled = true;
}

function Awake ()
{ enabled = false; }

function OnGUI ()
{
    gui_utils.centered_menu_header();

    GUILayout.BeginHorizontal(new GUIStyle(GUI.skin.box));

    GUILayout.BeginVertical();

    var title_style = new GUIStyle(GUI.skin.label);
    title_style.alignment = TextAnchor.MiddleCenter;
    GUILayout.Label('Select a team or hotseat play', title_style);

    GUILayout.BeginHorizontal();
    selection = GUILayout.SelectionGrid(selection, selection_names, 1, GUILayout.MinWidth(200));
    GUILayout.Label(selection_descriptions[selection], GUILayout.MaxWidth(300));
    GUILayout.EndHorizontal();

    GUILayout.BeginHorizontal();
    if (GUILayout.Button('Next')) {
        game_state_.new_game(scenario, selection_configs[selection]);
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
