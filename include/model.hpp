#pragma once

int init_nations (const char* nations_str);

int init_unit_defs (const char* unit_defs_str);

int init_scenario (const char* scenario_str);

int init_model (const char* map_str, const char* oob_str);

int reset_model ();

int save_model (const char* filename);

int load_model (const char* filename);
