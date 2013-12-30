#pragma strict


function clear ()
{
}

function new_game (scenario : SimpleJSON.JSONNode, player_config : Dictionary.<String, boolean>)
{
    print('TODO: Start new game.');
    for (var config : System.Collections.Generic.KeyValuePair.<String, boolean> in
         player_config) {
        print(config.Key + ' ' + (config.Value ? 'human' : 'AI'));
    }
}

function Start ()
{

}

function Update ()
{

}
