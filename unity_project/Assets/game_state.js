#pragma strict

import System.IO;
import RadicalRoutineExtensions;

@script SerializeAll

@DoNotSerialize
var game_data_ : game_data;
@DoNotSerialize
var setup_ui_ : setup_ui;

private var in_setup_ : boolean = true;
private var turn_ : int;
private var player_nations_ : Dictionary.<String, boolean>;
private var game_guid_ : String;

@DoNotSerialize
private static var this_ : game_state;

@DoNotSerialize
private var save_pending = false;

function save_async ()
{ save_pending = true; }

function turn ()
{ return turn_; }

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
    for (var nation in this_.game_data_.scenario().setup_order) {
        // TODO: Hotseat only for now.
        //if (this_.player_nations_[nation]) {
        if (true) {
            this_.setup_ui_.set_up(nation);
            while (this_.setup_ui_.enabled) {
                yield WaitForSeconds(0.1);
            }
            this_.save_async();
        } else {
            // TODO: wait for other player to set up this nation.
        }
    }
}

private static function game_main ()
{
    yield RadicalRoutineExtensions.StartExtendedCoroutine(this_, initial_setup());

    var iteration = 0;
    while (true) {
        print(String.Format('main {0}, {1} {2}', Time.time, ++iteration, this_.turn()));
        yield WaitForSeconds(1);
    }
}

function new_game (scenario_json : SimpleJSON.JSONNode, config : Dictionary.<String, boolean>)
{
    game_data_.load_data(scenario_json);

    player_nations_ = config;
    game_guid_ = System.Guid.NewGuid().ToString();

    while (!game_data_.map())
        yield WaitForSeconds(0.01);

    var scenario = game_data_.scenario();
    turn_ = scenario.start_turn;

    RadicalRoutineExtensions.StartExtendedCoroutine(gameObject, game_main());
}

function Awake ()
{ this_ = this; 
  Debug.Log(Application.persistentDataPath); }

function Update ()
{
    if (save_pending) {
        save();
        save_pending = false;
    }
}
