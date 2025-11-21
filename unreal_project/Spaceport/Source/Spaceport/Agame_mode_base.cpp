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
    Ugame_instance::get()->game_to_load(std::filesystem::path(*filename));
    multicast_load_playing();
}
