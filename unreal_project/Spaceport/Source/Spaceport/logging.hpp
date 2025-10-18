#pragma once

#include <filesystem>
#include <string>

#if defined(BUILD_FOR_TEST)
#define UE_LOG(category, verbosity, message, ...)
#define FSTRINGIZE(x)
#else
#include <Logging/StructuredLog.h>

#define FSTRINGIZE(x) fstringize(x)

inline FString fstringize(std::string s)
{
    return { UTF8_TO_TCHAR(s.c_str()) };
}

#if defined(_MSC_VER)
inline FString fstringize(std::wstring s)
{
    return { s.c_str() };
}
#endif

inline FString fstringize(std::filesystem::path p)
{
    return fstringize(p.native());
}
#endif
