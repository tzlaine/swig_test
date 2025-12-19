#include "lua.hpp"

#include <CoreMinimal.h>
#include <Misc/Paths.h>


namespace {
    void ue_log_impl(std::string const & msg)
    {
        UE_LOG(LogTemp, Log, TEXT("%s"), *FString(UTF8_TO_TCHAR(msg.c_str())));
    }
    void ue_warn_impl(std::string const & msg)
    {
        UE_LOG(
            LogTemp, Warning, TEXT("%s"), *FString(UTF8_TO_TCHAR(msg.c_str())));
    }
    void ue_err_impl(std::string const & msg)
    {
        UE_LOG(
            LogTemp, Error, TEXT("%s"), *FString(UTF8_TO_TCHAR(msg.c_str())));
    }
}

sol::state & lua()
{
    static sol::state retval;
    static bool once = true;
    if (once) {
        retval.open_libraries(
            sol::lib::base,
            sol::lib::package,
            sol::lib::string,
            sol::lib::math,
            sol::lib::table);

        retval.set_function("UE_LOG", &ue_log_impl);
        retval.set_function("UE_WARN", &ue_warn_impl);
        retval.set_function("UE_ERR", &ue_err_impl);

        once = false;
    }
    return retval;
}

std::string script_path(std::string const & script_)
{
    FString script(UTF8_TO_TCHAR(script_.c_str()));
    FString full_path =
        FPaths::Combine(*FPaths::ProjectContentDir(), TEXT("script"), *script);
    return std::string(TCHAR_TO_UTF8(*full_path));
}

void script_file(std::string const & script)
{
    lua().script_file(script_path(script));
}
