#pragma strict

var game_state_ : game_state;
var start_game_menus : GameObject;
var load_menu : GameObject;


function new_game ()
{
    game_state_.clear();
    gameObject.SetActive(false);
    start_game_menus.SetActive(true);
}

function load_game ()
{
    gameObject.SetActive(false);
    load_menu.SetActive(true);
}
