#include "Agame_mode_base.h"
#include "Agame_state_base.h"
#include "game_instance.h"
#include "utility.hpp"


namespace {
    Agame_state_base * cast(AGameStateBase * base)
    {
        return Cast<Agame_state_base>(base);
    }
}

Agame_mode_base::Agame_mode_base(FObjectInitializer const & init) :
    AGameModeBase(init)
{}

void Agame_mode_base::multicast_load_playing_Implementation()
{
    Ugame_instance::get()->load_playing_level();
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
    UE_LOG(
        LogTemp,
        Log,
        TEXT("Publish save files: %s"),
        *FString::Join(saves, TEXT(", ")));
    cast(GameState)->saves_ = std::move(saves);
    cast(GameState)->saves_changed();
}
