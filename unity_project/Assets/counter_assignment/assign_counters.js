#pragma strict

import SimpleJSON;
import System.IO;
import System.Collections.Generic;

var counter_sheet_column_ : counter_sheet_column;


private var columns : counter_sheet_column[] = new counter_sheet_column[5];
private var column_index : int = 0;

function assign (unit : String, texture : String, uv_ll : Vector2, uv_ur : Vector2)
{
}

function populate (from : String, col : int)
{
    var retval : counter_sheet_column = Instantiate(counter_sheet_column_);
    var files : String[] = [
        from + '_0_' + col,
        from + '_1_' + col,
        from + '_2_' + col,
        from + '_3_' + col,
        from + '_4_' + col,
        from + '_5_' + col,
        from + '_6_' + col
    ];
    retval.init(files);
    retval.gameObject.SetActive(false);
    return retval;
}

function change_column (forward : boolean)
{
    columns[column_index].gameObject.SetActive(false);
    column_index += forward ? 1 : columns.Length - 1;
    column_index = column_index % columns.Length;
    columns[column_index].gameObject.SetActive(true);
}

function next_column ()
{ change_column(true); }

function prev_column ()
{ change_column(false); }

function Start ()
{
    columns[0] = populate('alpha_front_cut', 0);
    columns[1] = populate('alpha_front_cut', 1);
    columns[2] = populate('bravo_front_cut', 0);
    columns[3] = populate('bravo_front_cut', 1);
    columns[4] = populate('bases_front_cut', 0);

    columns[column_index].gameObject.SetActive(true);

    var json : SimpleJSON.JSONNode = JSON.Parse(System.IO.File.ReadAllText('../units.json'));

    
}

function Update ()
{
    
}
