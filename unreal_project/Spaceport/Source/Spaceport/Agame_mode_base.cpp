#include "Agame_mode_base.h"
#include "Agame_state.h"
#include "Aplayer_state.h"
#include "game_instance.h"
#include "utility.hpp"


namespace {
    Agame_state* cast(AGameStateBase * base)
    {
        return Cast<Agame_state>(base);
    }
}

Agame_mode_base::Agame_mode_base(FObjectInitializer const & init) :
    AGameModeBase(init)
{}

void Agame_mode_base::PostLogin(APlayerController * player)
{
    Super::PostLogin(player);

    UE_LOG(LogTemp, Log, TEXT("Player logged in."));

    if (!player)
        return;

    auto * ps = Cast<Aplayer_state>(player->PlayerState);
    if (!ps)
        return;

    ps->player_id(players_);
    ++players_;
}

void Agame_mode_base::multicast_load_playing_Implementation()
{
    Ugame_instance::get()->load(level::playing);
}

void Agame_mode_base::load_and_start_game_Implementation(
    FString const & filename)
{
    auto f = to_path(filename);
    f += TEXT(".sav");
    Ugame_instance::get()->game_to_load(save_dir_path() / f);
    multicast_load_playing();
}

void Agame_mode_base::publish_save_files()
{
    TArray<FString> saves = find_save_files();

    // Filter out known non-game .sav files.
    auto const first = begin(saves);
    auto const last = end(saves);
    auto it = std::remove_if(first, last, [](auto const & e) {
        return e == TEXT("EnhancedInputUserSettings");
    });
    if (it != last)
        saves.SetNum(saves.Num() - (last - it));

    UE_LOG(
        LogTemp,
        Log,
        TEXT("Publish save files: %s"),
        *FString::Join(saves, TEXT(", ")));
    cast(GameState)->saves_ = std::move(saves);
    cast(GameState)->saves_changed();
}
