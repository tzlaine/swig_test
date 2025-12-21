#pragma once

// Must come before defining check, since it deals with the check macro
// explicitly.
#include "lua.hpp"

#if defined(BUILD_FOR_TEST)
#include <cassert>
#define check(...) assert(__VA_ARGS__)
#else
#include <Misc/AssertionMacros.h>
#endif
