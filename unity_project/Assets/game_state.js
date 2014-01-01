#pragma strict

import System.IO;
import RadicalRoutineExtensions;

@script SerializeAll

@DoNotSerialize
var game_data_ : game_data;

private var in_setup_ : boolean = true;
private var turn_ : int;
private var player_nations_ : String[];
private var setting_up_nation_ : int = -1;
private var game_guid_ : String;
private static var instance_ : game_state;

function turn ()
{ return turn_; }

private static function instance () : game_state
{ return instance_; }

private function turn_to_str (t : int) : String
{
    var retval : String = t / 10 == 0 ? 'Spring' : 'Fall';
    retval += ', Y' + Mathf.FloorToInt(t / 10.0);
    return retval;
}

private function save_description () : String
{
    var retval : String = game_data_.scenario().name;
    if (in_setup_)
        retval += ' - Initial setup';
    else
        retval += ' - ' + turn_to_str(turn_);
    return retval;
}

private function save ()
{
    var info = new DirectoryInfo(Application.persistentDataPath);
    var files = info.GetFiles();
    var matching_guid_saves = new Array();
    for (file in files) {
        var filename : String = file.ToString();
        if (filename.StartsWith(game_guid_))
            matching_guid_saves.Push(filename);
    }

    var save_filename : String = game_guid_;
    JSONLevelSerializer.SerializeLevelToFile(game_guid_ + save_description());

    for (match in matching_guid_saves) {
        File.Delete(match);
    }
}

function clear ()
{
    // TODO
}

private static function initial_setup ()
{
    // TODO: Show note about initial setup?

    var iteration = 0;
    while (iteration < 10) {
        print(String.Format('setup {0}, {1} {2}', Time.time, ++iteration, instance().turn()));
        yield WaitForSeconds(1);
    }
}

private static function game_main ()
{
    yield RadicalRoutineExtensions.StartExtendedCoroutine(instance(), initial_setup());

    var iteration = 0;
    while (true) {
        print(String.Format('main {0}, {1} {2}', Time.time, ++iteration, instance().turn()));
        yield WaitForSeconds(1);
    }
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

    game_guid_ = System.Guid.NewGuid().ToString();

    player_nations_ = new String[played_nations.length];
    for (var i = 0; i < played_nations.length; ++i) {
        player_nations_[i] = played_nations[i];
    }

    if (!game_data_.map())
        yield WaitForSeconds(0.01);

    var scenario = game_data_.scenario();
    turn_ = scenario.start_turn;

    RadicalRoutineExtensions.StartExtendedCoroutine(gameObject, game_main());

    save();
}

function Awake ()
{ instance_ = this; }

function Update ()
{
    // TODO
}
