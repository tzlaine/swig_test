#pragma strict

var hex_border_gaps : procedural_hex_gaps;
var game_data_ : game_data;
var place_tiles_ : place_tiles;

private var map_width_ : int = 0;
private var map_height_ : int = 0;
private var map_origin : Vector3 = Vector3(0, 0, 0);

private static var sin_60 : float = Mathf.Sin(Mathf.Deg2Rad * 60);

class border_gap
{
    public function border_gap () {}
    public function border_gap (edge_ : int,
                                first_vertex_ : boolean,
                                second_vertex_ : boolean,
                                first_gap_type_ : int,
                                second_gap_type_ : int)
    {
        edge = edge_;
        first_vertex = first_vertex_;
        second_vertex = second_vertex_;
        first_gap_type = first_gap_type_;
        second_gap_type = second_gap_type_;
    }
    var edge : int;
    var first_vertex : boolean;
    var second_vertex : boolean;
    var first_gap_type : int; // 0, 1, or 2 as in adjacencies
    var second_gap_type : int; // 0, 1, or 2 as in adjacencies
};

function place_tile (h : hex, m : map_t)
{
    var adjacencies_ : int[] = place_tiles_.adjacencies(h, m);
    var gaps : List.<border_gap> = new List.<border_gap>();
    var total_gaps : int = 0;
    for (var i = 0; i < adjacencies_.Length; ++i) {
        var adj = adjacencies_[i];
        if (adj == 0 || adj == 2) {
            var prev_adj = adjacencies_[(i + 5) % 6];
            var next_adj = adjacencies_[(i + 1) % 6];
            if (adj < prev_adj)
                ++total_gaps;
            if (adj < next_adj)
                ++total_gaps;
            if (adj < prev_adj || adj < next_adj) {
                gaps.Add(border_gap(i, adj < prev_adj, adj < next_adj, prev_adj, next_adj));
            }
        }
    }
    if (total_gaps) {
        var obj : procedural_hex_gaps = Instantiate(hex_border_gaps);
        obj.init(h.owner_id, total_gaps, gaps);
        obj.transform.position =
            Vector3(h.x * 1.5, (map_height_ - 1 - h.y) * 2 * sin_60, 0) - map_origin;
        if (h.x % 2 == 1)
            obj.transform.position.y -= sin_60;
        obj.renderer.material.renderQueue = 20;
    }
}

function Start ()
{
    var m : map_t = game_data_.map();

    map_width_ = m.hexes.GetLength(0);
    map_height_ = m.hexes.GetLength(1);

    map_origin = Vector3((map_width_ - 1) * 1.5 / 2,
                         (map_height_ - 1) * 2 * sin_60 / 2,
                         0);

    for (var x = 0; x < m.hexes.GetLength(0); ++x) {
        for (var y = 0; y < m.hexes.GetLength(1); ++y) {
            place_tile(m.hexes[x, y], m);
        }
    }
}

private var hexes_combined = false;

function Update ()
{
    if (!hexes_combined &&
        place_tiles_.combine_hexes(GetComponent(MeshFilter),
                                   FindObjectsOfType(procedural_hex_gaps))) {
        renderer.material.renderQueue = 20;
        hexes_combined = true;
    }
}
