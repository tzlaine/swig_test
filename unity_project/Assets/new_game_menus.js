#pragma strict

var select_scenario : GameObject;
var select_team : GameObject;
var place_fleets : GameObject;


function reset ()
{
    select_scenario.SetActive(true);
    select_team.SetActive(false);
    place_fleets.SetActive(false);
}
