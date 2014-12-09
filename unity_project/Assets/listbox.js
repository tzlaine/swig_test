#pragma strict

var panel : RectTransform;
var horizontal : boolean;


function set_data (gos : GameObject[], row_size : int)
{
//    scrollbar.value = 0;
    panel.DetachChildren();

//    scrollbar.numberOfSteps = gos.Length;

    if (horizontal) {
        panel.offsetMin.x = 0;
        panel.offsetMax.x = row_size * gos.Length;
    } else {
        panel.offsetMin.y = 0;
        panel.offsetMax.y = row_size * gos.Length;
    }

    for (var i = 0; i < gos.Length; ++i) {
        gos[i].transform.SetParent(panel);
        gos[i].transform.localScale = Vector3.one;
    }
}
