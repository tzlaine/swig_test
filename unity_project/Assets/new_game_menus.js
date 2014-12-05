#pragma strict

var select_scenario_ui : select_scenario;
var setup_game_ui : setup_game;


function OnEnabled ()
{
    select_scenario_ui.gameObject.SetActive(true);
    setup_game_ui.gameObject.SetActive(false);
}
