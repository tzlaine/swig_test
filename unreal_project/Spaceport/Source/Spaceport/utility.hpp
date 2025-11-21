#pragma once

#include "game_instance.h"
#include "Aplayer_controller.h"
#include "Aplaying_hud.h"
#include "huds/Amain_menu_controller.h"

#include <filesystem>

#include <HAL/FileManager.h>


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
    buf.SetNum(as_protobuf.ByteSize());
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

inline Aplayer_controller_base * player_controller_base()
{
    return Cast<Aplayer_controller_base>(::world()->GetFirstPlayerController());
}

inline Amain_menu_controller * main_menu_controller()
{
    return Cast<Amain_menu_controller>(::world()->GetFirstPlayerController());
}

inline Aplayer_controller * player_controller()
{
    return Cast<Aplayer_controller>(::world()->GetFirstPlayerController());
}

inline Aplaying_hud * playing_hud()
{
    auto * pc = ::world()->GetFirstPlayerController();
    if (!pc)
        return nullptr;
    return Cast<Aplaying_hud>(pc->GetHUD());
}

inline Ahud_base * hud_base()
{
    auto * pc = ::world()->GetFirstPlayerController();
    if (!pc)
        return nullptr;
    return Cast<Ahud_base>(pc->GetHUD());
}

inline Ahud_base * hud_base(AHUD * hud)
{
    if (!hud)
        return nullptr;
    return Cast<Ahud_base>(hud);
}
