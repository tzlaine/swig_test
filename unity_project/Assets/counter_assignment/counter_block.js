#pragma strict

function init (files : String[])
{
    var piece : counter_block_piece;

    piece = transform.GetChild(0).gameObject.AddComponent(counter_block_piece);
    piece.init(files[2]);

    piece = transform.GetChild(1).gameObject.AddComponent(counter_block_piece);
    piece.init(files[1]);

    piece = transform.GetChild(2).gameObject.AddComponent(counter_block_piece);
    piece.init(files[3]);

    piece = transform.GetChild(3).gameObject.AddComponent(counter_block_piece);
    piece.init(files[0]);
}
