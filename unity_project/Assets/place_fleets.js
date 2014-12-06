#pragma strict

@script SerializeAll

@DoNotSerialize
var game_data_ : game_data;
@DoNotSerialize
var game_state_ : game_state;
@DoNotSerialize
var place_hexes_ : place_hexes;
@DoNotSerialize
var map_click_listener_ : map_click_listener;
@DoNotSerialize
var counter_ : counter;

@DoNotSerialize
var title : UI.Text;
@DoNotSerialize
var counter_image : UI.Image;
@DoNotSerialize
var ships_list : listbox;
@DoNotSerialize
var fleets_list : listbox;
@DoNotSerialize
var ships_group : toggle_group;
@DoNotSerialize
var fleets_group : toggle_group;

@DoNotSerialize
private var pending_counters : Dictionary.<hex_coord, Array>;


