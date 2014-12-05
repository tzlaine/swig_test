#pragma strict

var new_game_menus : GameObject;
var start_menu : GameObject;
var select_team_ui : select_team;
var scenario_button : GameObject;
var scenarios_panel : RectTransform;
var current_scenario_description : UI.Text;
var scenario_button_size : float;
var spacing : float;

private var scenarios : JSONNode[];
private var scenario_names : String[];
private var scenario_descriptions : String[];


private function selection_changed (value : String)
{ current_scenario_description.text = scenario_descriptions[parseInt(value)]; }

function next ()
{
    var group : toggle_group = GetComponent(toggle_group);
    var selection : int = parseInt(group.value());
    select_team_ui.set_scenario(scenarios[selection]);

    gameObject.SetActive(false);
    select_team_ui.gameObject.SetActive(true);
}

function cancel ()
{
    new_game_menus.SetActive(false);
    start_menu.SetActive(true);
}

function OnEnable ()
{
    var data = new Array();

    var info = new DirectoryInfo('../scenarios');
    var files = info.GetFiles();

    scenarios = new JSONNode[files.Length];
    scenario_names = new String[files.Length];
    scenario_descriptions = new String[files.Length];

    scenarios_panel.DetachChildren();

    var group : toggle_group = GetComponent(toggle_group);
    group.notify = selection_changed;

    for (var i = 0; i < files.Length; ++i) {
        scenarios[i] = JSON.Parse(File.ReadAllText(files[i].ToString()));
        var name : String = scenarios[i]['name'];
        scenario_names[i] = name;
        var desc : String = scenarios[i]['description'];
        scenario_descriptions[i] = desc.Replace('\\n', '\n');

        var instance : GameObject = Instantiate(scenario_button);
        var text : UI.Text = instance.GetComponentInChildren(UI.Text);
        text.text = name;

        var selectable_item_ : selectable_item =
            instance.GetComponent(selectable_item);
        selectable_item_.value = i.ToString();
        selectable_item_.toggle_group_ = group;

        var toggle : UI.Toggle = instance.GetComponentInChildren(UI.Toggle);
        toggle.group = GetComponent(UI.ToggleGroup);

        instance.transform.SetParent(scenarios_panel);
        instance.transform.localScale = Vector3.one;

        if (i == 0) {
            toggle.isOn = true;
            current_scenario_description.text = scenario_descriptions[i];
        }
    }

    scenarios_panel.sizeDelta = Vector2(
        scenarios_panel.sizeDelta.x,
        scenario_button_size * files.Length + spacing * (files.Length - 1)
    );
}
