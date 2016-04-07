file(READ ${nations_file} nations_contents)
file(READ ${map_file} map_contents)
file(READ ${oob_file} oob_contents)
file(READ ${units_file} units_contents)
file(READ ${scenario_file} scenario_contents)

configure_file(${in_file} test_data.hpp)
