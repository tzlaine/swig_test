#pragma once

#include <filesystem>
#include <string>

#if defined(BUILD_FOR_TEST)
#define UE_LOG(category, verbosity, message, ...)
#define UE_CLOG(cond, category, verbosity, message, ...)
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

#define DEFINE_INLINE_LOG_CATEGORY(name)                                \
    inline struct FLogCategory ## name :                                \
    FLogCategory<ELogVerbosity::Log, ELogVerbosity::All> {              \
        FORCEINLINE FLogCategory ## name() :                            \
            FLogCategory(TEXT(#name)) {}                                \
    } name

DEFINE_INLINE_LOG_CATEGORY(general);
#endif
