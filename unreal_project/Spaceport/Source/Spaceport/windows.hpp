#pragma once

#if defined(BUILD_FOR_TEST)
#define WIN32_LEAN_AND_MEAN
#define UNICODE
#define _UNICODE
#include <Windows.h>
#if defined(min)
#undef min
#endif
#if defined(max)
#undef max
#endif
#if defined(advance)
#undef advance
#endif
#if defined(near)
#undef near
#endif
#if defined(far)
#undef far
#endif
#else
#include <Windows/WindowsHWrapper.h>
#endif
