﻿#pragma strict

var game_state_ : game_state;
var new_sp_game_setup_ : new_sp_game_setup;


function OnGUI ()
{
    var buttons : String[] = [
        'New Game',
        'Load Game'
    ];

    var button_height = 30;
    var button_width = 280;
    var margin = 10;
    var menu_width = button_width + 2 * margin;
    var menu_height = buttons.Length * button_height + (buttons.Length + 1) * margin;

    GUI.BeginGroup(Rect((Screen.width - menu_width) / 2,
                        (Screen.height - menu_height) / 2,
                        menu_width,
                        menu_height));

    var y = margin;
    var button_states = new boolean[buttons.Length];
    for (var i = 0; i < buttons.Length; ++i) {
        button_states[i] =
            GUI.Button(Rect(margin, y, button_width, button_height), buttons[i]);
        y += button_height + margin;
    }


    if (button_states[0]) {
        game_state_.clear();
        new_sp_game_setup_.setup();
        enabled = false;
    } else if (button_states[1]) {
        print('TODO: load game');
    }

    GUI.EndGroup();
}