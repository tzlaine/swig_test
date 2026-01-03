#pragma once

#include "game_instance.h"
#include "Aplayer_controller.h"
#include "Ahud_t.h"
#include "text/beman_utf_view/utf_view.hpp"

#include <boost/type_index.hpp>

#include <algorithm>
#include <exception>
#include <filesystem>
#include <string>

#include <HAL/FileManager.h>
#include <Sound/SoundMix.h>


#define LOG_FORMAT(fmt_str, ...)                                               \
    UE_LOG(                                                                    \
        LogTemp,                                                               \
        Log,                                                                   \
        TEXT("%s"),                                                            \
        *FString(std::format(fmt_str __VA_OPT__(, ) __VA_ARGS__).c_str()))
#define WARN_FORMAT(fmt_str, ...)                                              \
    UE_LOG(                                                                    \
        LogTemp,                                                               \
        Warning,                                                               \
        TEXT("%s"),                                                            \
        *FString(std::format(fmt_str __VA_OPT__(, ) __VA_ARGS__).c_str()))
#define ERROR_FORMAT(fmt_str, ...)                                             \
    UE_LOG(                                                                    \
        LogTemp,                                                               \
        Error,                                                                 \
        TEXT("%s"),                                                            \
        *FString(std::format(fmt_str __VA_OPT__(, ) __VA_ARGS__).c_str()))

inline constexpr float meters = 100.0f;

template<typename T>
void call_real_soon(FTimerHandle & timer, T * t_ptr, void (T::*member_ptr)())
{
    t_ptr->GetWorldTimerManager().SetTimer(
        timer, t_ptr, member_ptr, 0.001, false);
}

inline std::filesystem::path save_dir_path()
{
    return std::filesystem::path(
        *(FPaths::ProjectSavedDir() + TEXT("SaveGames/")));
}

inline TArray<FString> find_save_files(bool keep_extension = false)
{
    FString dir = FPaths::ProjectSavedDir() + TEXT("SaveGames/");
    TArray<FString> saves;
    IFileManager::Get().FindFiles(saves, *dir, TEXT("*.sav"));
    if (!keep_extension) {
        for (auto & save : saves) {
            save.RemoveFromEnd(TEXT(".sav"));
        }
    }
    return std::move(saves);
}

inline std::filesystem::path to_path(FString const & s)
{
#if defined(_MSC_VER)
    return std::filesystem::path(*s);
#else
#error "to_path(FString const & s) needs an implementation for this platform"
#endif
}

inline FString to_fstring(std::filesystem::path const & p)
{
#if defined(_MSC_VER)
    return FString(p.c_str());
#else
#error "to_fstring(std::filesystem::path const & s) needs an implementation for this platform"
#endif
}

inline std::string to_string(FString::ElementType const * p)
{
    auto utf8 = beman::utf_view::null_term(p) | beman::utf_view::to_utf8;
    std::string retval;
    for (auto c : utf8) {
        retval += c;
    }
    return retval;
}

inline std::string to_string(FString const & s)
{
    auto utf8 =
        std::ranges::subrange(*s, *s + s.Len()) | beman::utf_view::to_utf8;
    std::string retval;
    for (auto c : utf8) {
        retval += c;
    }
    return retval;
}

template<>
struct std::formatter<FString>
{
    constexpr auto parse(std::format_parse_context & ctx)
    {
        return ctx.begin();
    }

    template<typename Ctx>
    auto format(FString const & s, Ctx & ctx) const
    {
        auto utf8 =
            std::ranges::subrange(*s, *s + s.Len()) | beman::utf_view::to_utf8;
        return std::ranges::copy(utf8, ctx.out()).out;
    }
};

template<>
struct std::formatter<FName>
{
    constexpr auto parse(std::format_parse_context & ctx)
    {
        return ctx.begin();
    }

    template<typename Ctx>
    auto format(FName const & n, Ctx & ctx) const
    {
        using char_type = FString::ElementType;
        char_type buf[FName::StringBufferSize + 1];
        char_type const * const last = buf + n.ToString(buf);
        auto utf8 =
            std::ranges::subrange(buf, last) | beman::utf_view::to_utf8;
        return std::ranges::copy(utf8, ctx.out()).out;
    }
};

template<typename T>
T * begin(TArray<T> & a)
{
    return a.GetData();
}
template<typename T>
T * end(TArray<T> & a)
{
    return a.GetData() + a.Num();
}
template<typename T>
T const * begin(TArray<T> const & a)
{
    return a.GetData();
}
template<typename T>
T const * end(TArray<T> const & a)
{
    return a.GetData() + a.Num();
}

template<typename T>
TArray<uint8> to_tarray(T const & x)
{
    auto as_protobuf = to_protobuf(x);
    TArray<uint8> buf;
    buf.SetNum(as_protobuf.ByteSizeLong());
    as_protobuf.SerializeWithCachedSizesToArray(buf.GetData());
    return std::move(buf);
}

template<typename T>
T from_tarray(TArray<uint8> const & buf)
{
    decltype(to_protobuf(std::declval<T>())) as_protobuf;
    as_protobuf.ParseFromArray(buf.GetData(), buf.Num());
    return from_protobuf(as_protobuf);
}

inline Aplayer_controller * player_controller()
{
    return Cast<Aplayer_controller>(::world()->GetFirstPlayerController());
}

inline Ahud_t * hud()
{
    auto * pc = ::world()->GetFirstPlayerController();
    if (!pc)
        return nullptr;
    return Cast<Ahud_t>(pc->GetHUD());
}

inline Ahud_t * hud(AHUD * hud)
{
    if (!hud)
        return nullptr;
    return Cast<Ahud_t>(hud);
}

inline USoundClass * sound_class_of(USoundMix const * mix)
{
    if (!mix)
        return nullptr;
    for (FSoundClassAdjuster const & o : mix->SoundClassEffects) {
        if (o.SoundClassObject)
            return o.SoundClassObject;
    }
    return nullptr;
}

template<typename T>
void set_property(AActor * a, FName name, T value)
{
#if 0 // TODO
    TMap<FName, FString> all_props;
    for (TFieldIterator<FProperty> it(
             a->GetClass(), EFieldIteratorFlags::ExcludeSuper);
         it;
         ++it) {
        FProperty * prop = *it;

        if (prop->IsA<FObjectProperty>())
            continue;

        FString value_str;
        if (prop->ExportText_InContainer(0, value_str, a, a, a, PPF_None))
            all_props.Add(prop->GetFName(), value_str);
    }
#endif

    FProperty * const p = a->GetClass()->FindPropertyByName(name);
    if (!p) {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("%s"),
            *FString(
                std::format(
                    "Actor {} does not have property {}.", a->GetName(), name)
                    .c_str()));
        return;
    }

    if constexpr (
        std::same_as<T, bool> || std::same_as<T, int> ||
        std::same_as<T, FVector> || std::same_as<T, FLinearColor> ||
        std::same_as<T, UTexture *>) {
        if (T * const ptr = p->ContainerPtrToValuePtr<T>(a)) {
            *ptr = std::move(value);
        } else {
            UE_LOG(
                LogTemp,
                Error,
                TEXT("%s"),
                *FString(std::format(
                             "Could not get property {} of type {} from "
                             "actor {}.",
                             name,
                             boost::typeindex::type_id<T>().pretty_name(),
                             a->GetName())
                             .c_str()));
        }
        return;
    }

    if constexpr (std::same_as<T, double> || std::same_as<T, float>) {
        if (FDoubleProperty * double_p = CastField<FDoubleProperty>(p)) {
            if (double * const ptr =
                    double_p->ContainerPtrToValuePtr<double>(a)) {
                *ptr = std::move(value);
                return;
            } else {
                UE_LOG(
                    LogTemp,
                    Error,
                    TEXT("%s"),
                    *FString(
                        std::format(
                            "Could not get property {} of type double from "
                            "actor {}.",
                            name,
                            a->GetName())
                            .c_str()));
            }
        } else {
            UE_LOG(
                LogTemp,
                Error,
                TEXT("%s"),
                *FString(std::format(
                             "Could not cast actor {}'s property {} to a "
                             "FDoubleProperty.",
                             a->GetName(),
                             name)
                             .c_str()));
        }
    }

    if constexpr (std::same_as<T, float>) {
        if (FFloatProperty * float_p = CastField<FFloatProperty>(p)) {
            if (float * const ptr = float_p->ContainerPtrToValuePtr<float>(a)) {
                *ptr = std::move(value);
            } else {
                UE_LOG(
                    LogTemp,
                    Error,
                    TEXT("%s"),
                    *FString(std::format(
                                 "Could not get property {} of type float from "
                                 "actor {}.",
                                 name,
                                 a->GetName())
                                 .c_str()));
            }
        } else {
            UE_LOG(
                LogTemp,
                Error,
                TEXT("%s"),
                *FString(std::format(
                             "Could not cast actor {}'s property {} to a "
                             "FFloatProperty.",
                             a->GetName(),
                             name)
                             .c_str()));
        }
    }
}

template<typename T>
bool double_clicked(
    T & prev_click_target,
    std::chrono::time_point<std::chrono::system_clock> & prev_click_time,
    T click_target,
    T invalid_click_target)
{
    auto const now = std::chrono::system_clock::now();
    if (prev_click_target == click_target &&
        (now - prev_click_time) <
            std::chrono::duration<float>(max_double_click_interval_s)) {
        prev_click_time = std::chrono::time_point<std::chrono::system_clock>();
        prev_click_target = invalid_click_target;
        return true;
    }
    prev_click_time = now;
    prev_click_target = click_target;
    return false;
}
