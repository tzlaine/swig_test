#pragma strict

var circle : GameObject;
var square : GameObject;

private static var section : GameObject = null;
private static var connector : GameObject = null;
private static var section_scale = 0.12 * 1.1;
private static var connector_width = 0.08 * 1.1;
private static var connector_length = 0.32 * 1.1;

function make_section ()
{
    var retval : GameObject = Instantiate(circle);
    var mesh : Mesh = retval.GetComponent(MeshFilter).mesh;
    var vertices : Vector3[] = mesh.vertices;
    for (var i = 0; i < mesh.vertexCount; ++i) {
        vertices[i].x *= section_scale;
        vertices[i].y *= section_scale;
    }
    mesh.vertices = vertices;
    retval.GetComponent.<Renderer>().sharedMaterial.renderQueue = 30;
    retval.SetActive(false);
    return retval;
}

function make_connector ()
{
    var retval : GameObject = Instantiate(square);
    var mesh : Mesh = retval.GetComponent(MeshFilter).mesh;
    var vertices : Vector3[] = mesh.vertices;
    for (var i = 0; i < mesh.vertexCount; ++i) {
        vertices[i].x *= connector_width;
        vertices[i].y *= connector_length;
    }
    mesh.vertices = vertices;
    retval.GetComponent.<Renderer>().sharedMaterial.renderQueue = 30;
    retval.SetActive(false);
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
    if (!section)
        section = make_section();
    if (!connector)
        connector = make_connector();

    var obj : GameObject = null;
    var mesh_filter : MeshFilter = null;
    var combine : CombineInstance[] = new CombineInstance[7];

    obj = Instantiate(section);
    set_owner(obj.GetComponent(MeshFilter).mesh, owner);
    obj.transform.parent = transform;
    mesh_filter = obj.GetComponent(MeshFilter);
    combine[6].mesh = mesh_filter.mesh;
    combine[6].transform = mesh_filter.transform.localToWorldMatrix;
    obj.SetActive(false);

    for (var i = 0; i < 3; ++i) {
        var theta_deg = 90.0 + 120.0 * i;
        var theta = Mathf.Deg2Rad * theta_deg;
        var d : float = connector_length;

        obj = Instantiate(section);
        obj.transform.localPosition =
            Vector3(d * Mathf.Cos(theta), d * Mathf.Sin(theta), 0);
        set_owner(obj.GetComponent(MeshFilter).mesh, owner);
        obj.transform.parent = transform;
        mesh_filter = obj.GetComponent(MeshFilter);
        combine[i * 2 + 0].mesh = mesh_filter.mesh;
        combine[i * 2 + 0].transform = mesh_filter.transform.localToWorldMatrix;
        obj.SetActive(false);

        obj = Instantiate(connector);
        obj.transform.position =
            Vector3(d / 2 * Mathf.Cos(theta), d / 2 * Mathf.Sin(theta), 0);
        obj.transform.Rotate(Vector3(0, 0, theta_deg - 90.0));
        set_owner(obj.GetComponent(MeshFilter).mesh, owner);
        obj.transform.parent = transform;
        mesh_filter = obj.GetComponent(MeshFilter);
        combine[i * 2 + 1].mesh = mesh_filter.mesh;
        combine[i * 2 + 1].transform = mesh_filter.transform.localToWorldMatrix;
        obj.SetActive(false);
    }

    mesh_filter = GetComponent(MeshFilter);
    var mesh = new Mesh();
    mesh_filter.mesh = mesh;
    mesh.CombineMeshes(combine);
}

function Awake ()
{ enabled = false; }
