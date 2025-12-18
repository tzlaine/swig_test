#pragma once

#define SOL_LUA_VERSION 504
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>


sol::state & lua();
