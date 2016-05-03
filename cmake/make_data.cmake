file(READ ${nations_file} nations_contents)
file(READ ${map_file} map_contents)
file(READ ${oob_file} oob_contents)
file(READ ${units_file} units_contents)
file(READ ${scenario_file} scenario_contents)
file(READ ${hex_map_config_file} hex_map_config_contents)

configure_file(${in_file} test_data.hpp)
