#pragma strict

var circle : GameObject;
var square : GameObject;

private static var center : GameObject = null;
private static var satellite : GameObject = null;
private static var center_scale = 0.3;
private static var satellite_scale = 0.12;

function make_circle (scale : float)
{
    var retval : GameObject = Instantiate(circle);
    var mesh : Mesh = retval.GetComponent(MeshFilter).mesh;
    var vertices : Vector3[] = mesh.vertices;
    for (var i = 0; i < mesh.vertexCount; ++i) {
        vertices[i].x *= scale;
        vertices[i].y *= scale;
    }
    mesh.vertices = vertices;
    retval.renderer.sharedMaterial.renderQueue = 30;
    return retval;
}

function set_owner (mesh : Mesh, owner : int)
{
    var uv2 : Vector2[] = new Vector2[mesh.vertexCount];
    for (var i = 0; i < mesh.vertexCount; ++i) {
        uv2[i] = Vector2(owner / 255.0, 1);
    }
    mesh.uv2 = uv2;
}

function init (owner : int)
{
    if (!center)
        center = make_circle(center_scale);
    if (!satellite)
        satellite = make_circle(satellite_scale);

    var obj : GameObject = null;
    var mesh_filter : MeshFilter = null;
    var combine : CombineInstance[] = new CombineInstance[7];

    obj = Instantiate(center);
    set_owner(obj.GetComponent(MeshFilter).mesh, owner);
    obj.transform.parent = transform;
    mesh_filter = obj.GetComponent(MeshFilter);
    combine[6].mesh = mesh_filter.mesh;
    combine[6].transform = mesh_filter.transform.localToWorldMatrix;
    obj.SetActive(false);

    for (var i = 0; i < 6; ++i) {
        obj = Instantiate(satellite);
        var theta = Mathf.Deg2Rad * (90.0 + 60.0 * i);
        var d : float = center_scale + satellite_scale;
        obj.transform.localPosition =
            Vector3(d * Mathf.Cos(theta), d * Mathf.Sin(theta), 0);
        set_owner(obj.GetComponent(MeshFilter).mesh, owner);
        obj.transform.parent = transform;
        mesh_filter = obj.GetComponent(MeshFilter);
        combine[i].mesh = mesh_filter.mesh;
        combine[i].transform = mesh_filter.transform.localToWorldMatrix;
        obj.SetActive(false);
    }

    mesh_filter = GetComponent(MeshFilter);
    var mesh = new Mesh();
    mesh_filter.mesh = mesh;
    mesh.CombineMeshes(combine);
}
