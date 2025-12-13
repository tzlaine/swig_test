#pragma once

#if defined(_MSC_VER)
#define PUSH_WARNING __pragma(warning(push))
#define DISABLE_WARNING(n) __pragma(warning(disable : n))
#define PUSH_DISABLE_WARNING(n) PUSH_WARNING DISABLE_WARNING(n)
#define POP_WARNING __pragma(warning(pop))
#else
#define PUSH_WARNING
#define DISABLE_WARNING(n)
#define PUSH_DISABLE_WARNING(n)
#define POP_WARNING
#endif
