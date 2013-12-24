#pragma strict

private var file : String;

function init (file_ : String)
{
    file = file_;
    renderer.material.mainTexture =
        AssetDatabase.LoadAssetAtPath('Assets/counters/' + file + '.png', Texture2D);
}

function OnMouseUpAsButton ()
{
    print('hit on ' + file);
}
