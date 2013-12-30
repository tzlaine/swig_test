#pragma strict

import System.IO;

@script SerializeAll

@DoNotSerialize
var game_data_ : game_data;

private var in_setup : boolean = true;
private var turn : int;
private var player_nations : String[];
private var setting_up_nation : int = -1;
private var game_guid : String;

private function turn_to_str (turn_ : int) : String
{
    var retval : String = turn_ / 10 == 0 ? 'Spring' : 'Fall';
    retval += ', Y' + Mathf.FloorToInt(turn_ / 10.0);
}

private function save_description () : String
{
    var retval : String = game_data_.scenario().name;
    if (in_setup)
        retval += ' - Initial setup';
    else
        retval += ' - ' + turn_to_str(turn);
    return retval;
}

private function save ()
{
    var info = new DirectoryInfo(Application.persistentDataPath);
    var files = info.GetFiles();
    var matching_guid_saves = new Array();
    for (file in files) {
        var filename : String = file.ToString();
        if (filename.StartsWith(game_guid))
            matching_guid_saves.Push(filename);
    }

    var save_filename : String = game_guid;
    JSONLevelSerializer.SerializeLevelToFile(game_guid + save_description());

    for (match in matching_guid_saves) {
        File.Delete(match);
    }
}

function clear ()
{
    // TODO
}

function new_game (scenario_json : SimpleJSON.JSONNode, config : Dictionary.<String, boolean>)
{
    var played_nations = new Array();
    for (var conf : System.Collections.Generic.KeyValuePair.<String, boolean> in
         config) {
        // TODO: Restore this later; for now, we play hotseat-only. if (conf.Value)
            played_nations.Push(conf.Key);
    }
    game_data_.load_data(scenario_json);

    game_guid = System.Guid.NewGuid().ToString();

    player_nations = new String[played_nations.length];
    for (var i = 0; i < played_nations.length; ++i) {
        player_nations[i] = played_nations[i];
    }

    // TODO: Show note about initial setup.

    if (!game_data_.map())
        yield WaitForSeconds(0.01);

    var scenario = game_data_.scenario();
    turn = scenario.start_turn;

    save();
}

function Update ()
{
    // TODO
}
