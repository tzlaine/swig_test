#pragma strict

var hex_surface : GameObject;

function Start ()
{
    var sin_60 = Mathf.Sin(Mathf.Deg2Rad * 60);

    var red : boolean = false;
    for (var y : int = 0; y < 5; y++) {
        for (var x : int = 0; x < 5; x++) {
            var obj : GameObject = Instantiate(hex_surface);
            obj.transform.position = Vector3(x * 1.5, y * 2 * sin_60, 0);
            if (x % 2 == 1)
                obj.transform.position.y -= sin_60;
            obj.renderer.material.color = red ? Color.white : Color.red;
            red = !red;
        }
    }
}

function Update ()
{

}
