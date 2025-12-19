#pragma once

#define SOL_LUA_VERSION 504
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include <exception>


sol::state & lua();

std::string script_path(std::string const & script);

void script_file(std::string const & script);
