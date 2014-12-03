#pragma strict

var panel_rt : RectTransform;
var scrollbar : UI.Scrollbar;


private function reset ()
{
    scrollbar.value = 0;
    panel_rt.DetachChildren();
    panel_rt.anchorMin.y = 0;
    panel_rt.anchorMax.y = 0;
}

function Start ()
{ reset(); }

function set_data (objects : RectTransform[])
{
    reset();

    scrollbar.numberOfSteps = objects.Length;

    var h : float = 0;
    for (object in objects) {
        object.SetParent(panel_rt);
        h += object.rect.height;
    }

    panel_rt.anchorMax.y = h;
}
