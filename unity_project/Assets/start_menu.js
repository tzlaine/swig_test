#pragma strict

var game_state_ : game_state;
var new_sp_game_setup_ : new_sp_game_setup;
var load_menu : GameObject;


function new_game ()
{
    game_state_.clear();
    new_sp_game_setup_.setup();
    gameObject.SetActive(false);
}

function load_game ()
{
    gameObject.SetActive(false);
    load_menu.SetActive(true);
}
