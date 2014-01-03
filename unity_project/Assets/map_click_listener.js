#pragma strict

var offmap_clicked_callback : System.Action.<String>;
var hex_clicked_callback : System.Action.<hex_coord>;

function offmap_clicked (owner : String)
{
    print('offmap for owner ' + owner);
    if (offmap_clicked_callback)
        offmap_clicked_callback(owner);
}

function hex_clicked (hc : hex_coord)
{
    var hc2 : hex_coord = hex_coord(hc.x + 1, hc.y + 1);
    print('hex ' + (hc2.x < 10 ? '0' : '') + (hc2.x * 100 + hc2.y) +
          ' (' + hc.x + ',' + hc.y + ')');
    if (hex_clicked_callback)
        hex_clicked_callback(hc);
}
