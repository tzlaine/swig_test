#pragma strict

static function centered_menu_header ()
{
    GUILayout.BeginArea(Rect(0, 0, Screen.width, Screen.height));
    GUILayout.BeginVertical();
    GUILayout.FlexibleSpace();
    GUILayout.BeginHorizontal();
    GUILayout.FlexibleSpace();
}

static function centered_menu_footer ()
{
    GUILayout.FlexibleSpace();
    GUILayout.EndHorizontal();
    GUILayout.FlexibleSpace();
    GUILayout.EndVertical();
    GUILayout.EndArea();
}
