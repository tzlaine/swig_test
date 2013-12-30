#pragma strict

import gui_utils;

var game_state_ : game_state;
var new_sp_game_setup_ : new_sp_game_setup;


function OnGUI ()
{
    var buttons : String[] = [
        'New Game',
        'Load Game'
    ];

    gui_utils.centered_menu_header();

    GUILayout.BeginVertical(new GUIStyle(GUI.skin.box));

    var button_states = new boolean[buttons.Length];
    for (var i = 0; i < buttons.Length; ++i) {
        button_states[i] =
            GUILayout.Button(buttons[i], GUILayout.MaxWidth(200));
    }

    if (button_states[0]) {
        game_state_.clear();
        new_sp_game_setup_.setup();
        enabled = false;
    } else if (button_states[1]) {
        print('TODO: load game');
    }

    GUILayout.EndVertical();

    gui_utils.centered_menu_footer();
}
