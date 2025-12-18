#include "lua.hpp"


sol::state & lua()
{
    static sol::state retval;
    static bool once = true;
    if (once) {
        retval.open_libraries(sol::lib::base, sol::lib::package);
        once = false;
    }
    return retval;
}
