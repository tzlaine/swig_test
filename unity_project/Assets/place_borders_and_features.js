#pragma strict

var hex_borders : procedural_borders;
var circle : GameObject;
var star : procedural_star;
var starbase : procedural_starbase;
var battlestation : procedural_battlestation;
var place_hexes_ : place_hexes;
var game_data_ : game_data;


function adjacency (h : hex_t, h2 : hex_t) : int
{
    if (!h2 || h.owner_id != h2.owner_id)
        return 2;
    if (h.province != h2.province)
        return 1;
    return 0;
}

function adjacencies (h : hex_t, m : map_t) : int[]
{
    var retval : int[] = new int[6];
    retval[0] = adjacency(h, game_data.hex_above_right(h, m));
    retval[1] = adjacency(h, game_data.hex_above(h, m));
    retval[2] = adjacency(h, game_data.hex_above_left(h, m));
    retval[3] = adjacency(h, game_data.hex_below_left(h, m));
    retval[4] = adjacency(h, game_data.hex_below(h, m));
    retval[5] = adjacency(h, game_data.hex_below_right(h, m));
    return retval;
}

function make_feature (feature : String, owner : String, position : Vector3)
{
    if (feature == 'none')
        return;

    var scale : float = 1.0;
    var owner_id = game_data_.id(owner);

    if (feature == 'capitol') {
        var capitol : procedural_star = Instantiate(star);
        capitol.init(owner_id, game_data_.capitol_star_points(owner));
        scale = 0.6;
        capitol.transform.localScale = Vector3(scale, scale, scale);
        capitol.renderer.sharedMaterial.renderQueue = 30;
        capitol.transform.position = position;
    } else if (feature == 'MIN' || feature == 'MAJ') {
        var planet : GameObject = Instantiate(circle);
        scale = feature == 'MIN' ? 0.2 : 0.3;
        var mesh : Mesh = planet.GetComponent(MeshFilter).mesh;
        var uv2 : Vector2[] = new Vector2[mesh.vertexCount];
        for (var i = 0; i < mesh.vertexCount; ++i) {
            uv2[i] = Vector2(owner_id / 255.0, 1);
        }
        mesh.uv2 = uv2;
        planet.renderer.material.renderQueue = 30;
        planet.transform.localScale = Vector3(scale, scale, -scale);
        planet.transform.position = position;
    } else if (feature == 'SB') {
        var sb : procedural_starbase = Instantiate(starbase);
        sb.init(owner_id);
        sb.transform.position = position;
    } else if (feature == 'BATS') {
        var bats : procedural_battlestation = Instantiate(battlestation);
        bats.init(owner_id);
        bats.transform.position = position;
    }
}

function place_features (h : hex_t)
{ make_feature(h.feature, h.owner, place_hexes_.hex_center(h.hc)); }

function Start ()
{
    var m : map_t = game_data_.map();
    while (!m) {
        yield WaitForSeconds(0.01);
        m = game_data_.map();
    }

    for (var x = 0; x < m.width; ++x) {
        for (var y = 0; y < m.height; ++y) {
            var hc = hex_coord(x, y);
            var h = m.hex(hc);
            var borders : procedural_borders = Instantiate(hex_borders);
            var adjacencies_ : int[] = adjacencies(h, m);
            borders.init(h.owner_id, adjacencies_);
            borders.transform.position = place_hexes_.hex_center(hc);
            borders.renderer.material.renderQueue = 20;

            place_features(h);
        }
    }
}

private var features_combined = false;

function combine_features (mesh_filter : MeshFilter, features : Component[]) : boolean
{
    if (!features.Length)
        return false;

    var mesh = new Mesh();
    mesh_filter.mesh = mesh;

    var combine : CombineInstance[] = new CombineInstance[features.Length];
    for (var i = 0; i < features.Length; i++){
        var mf : MeshFilter = features[i].GetComponent(MeshFilter);
	combine[i].mesh = mf.mesh;
	combine[i].transform = mf.transform.localToWorldMatrix;
	mf.gameObject.SetActive(false);
    }
    mesh.CombineMeshes(combine);

    return true;
}

function Update ()
{
    var features : Component[] = null;
    if (!features_combined) {
        var dest = 0;
        var borders : Component[] = FindObjectsOfType(procedural_borders);
        var stars : Component[] = FindObjectsOfType(procedural_star);
        var starbases : Component[] = FindObjectsOfType(procedural_starbase);
        var battlestations : Component[] = FindObjectsOfType(procedural_battlestation);
        features = new Component[borders.Length + stars.Length + starbases.Length + battlestations.Length];
        System.Array.Copy(borders, 0, features, dest, borders.Length);
        dest += borders.Length;
        System.Array.Copy(stars, 0, features, dest, stars.Length);
        dest += stars.Length;
        System.Array.Copy(starbases, 0, features, dest, starbases.Length);
        dest += starbases.Length;
        System.Array.Copy(battlestations, 0, features, dest, battlestations.Length);
        dest += battlestations.Length;
    }
    if (features && features.Length && combine_features(GetComponent(MeshFilter), features)) {
        renderer.material.renderQueue = 30;
        features_combined = true;
    }
}
