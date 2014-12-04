#pragma strict

var panel_rt : RectTransform;
var scrollbar : UI.Scrollbar;


function set_data (rts : RectTransform[], row_height : int)
{
    scrollbar.value = 0;
    panel_rt.DetachChildren();
    panel_rt.anchorMin.y = 0;
    panel_rt.anchorMax.y = 0;

    scrollbar.numberOfSteps = rts.Length;

    var h : float = 0;
Debug.Log('panel_rt=' + panel_rt);
    for (var i = 0; i < rts.Length; ++i) {
Debug.Log('object=' + rts[i] + ' parent=' + rts[i].parent);
        rts[i].SetParent(panel_rt);
        rts[i].localScale = Vector3.one;
Debug.Log('  object=' + rts[i] + ' parent=' + rts[i].parent);
        h += row_height;
    }

    panel_rt.anchorMax.y = h;

Debug.Log('h=' + h);
Debug.Log('panel_rt.rect=' + panel_rt.rect);
}
