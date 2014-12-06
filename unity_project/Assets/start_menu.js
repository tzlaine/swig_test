#pragma strict

@script DoNotSerializePublic

var game_state_ : game_state;
var new_game_menus_ui : new_game_menus;
var load_menu : GameObject;


function new_game ()
{
    game_state_.clear();
    gameObject.SetActive(false);
    new_game_menus_ui.reset();
    new_game_menus_ui.gameObject.SetActive(true);
}

function load_game ()
{
    gameObject.SetActive(false);
    load_menu.SetActive(true);
}
