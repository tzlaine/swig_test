#include "Agame_mode.h"
#include "Agame_state.h"
#include "Amap_hud.h"
#include "Aplayer_controller.h"
#include "game_instance.h"

#include <filesystem>


namespace {
    Agame_state * cast(AGameStateBase * base)
    {
        return Cast<Agame_state>(base);
    }
}

Agame_mode::Agame_mode(FObjectInitializer const & init) :
    AGameModeBase(init)
{
    HUDClass = Amap_hud::StaticClass();
    PlayerControllerClass = Aplayer_controller::StaticClass();
    GameStateClass = Agame_state::StaticClass();
}

void Agame_mode::start_sp_game()
{
    std::filesystem::path load_path = Ugame_instance::get()->game_to_load();
    if (load_path.empty()) {
        cast(GameState)->play_state_ = play_state::setup;
        cast(GameState)->play_state_changed();
    } else {
        // TODO game_data_.load(load_path);
    }
}

void Agame_mode::start_mp_game()
{
    // TODO
}

void Agame_mode::BeginPlay()
{
    Super::BeginPlay();

    if (Ugame_instance::get()->game_kind() == game_kind::sp)
        start_sp_game();
    else
        start_mp_game();
}
