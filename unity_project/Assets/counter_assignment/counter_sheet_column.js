#pragma strict

function init (files_root : String[])
{
    var files : String[] = new String[4];
    var counter_blocks : counter_block[] =
        gameObject.GetComponentsInChildren.<counter_block>();
    for (var i = 0; i < counter_blocks.Length; ++i) {
        files[0] = files_root[i] + '.0';
        files[1] = files_root[i] + '.1';
        files[2] = files_root[i] + '.2';
        files[3] = files_root[i] + '.3';
        counter_blocks[i].init(files);
    }
}
