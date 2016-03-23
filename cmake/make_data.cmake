file(READ ${nations_file} nations_contents)
file(READ ${map_file} map_contents)
file(READ ${oob_file} oob_contents)

configure_file(${in_file} data.hpp)
