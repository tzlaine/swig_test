#include "Agame_mode.h"
#include "Agame_state.h"
#include "Aplaying_hud.h"
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
    UE_LOG(LogTemp, Log, TEXT("ENTER Agame_mode CTOR"));
    HUDClass = Aplaying_hud::StaticClass();
    PlayerControllerClass = Aplayer_controller::StaticClass();
    GameStateClass = Agame_state::StaticClass();
    UE_LOG(LogTemp, Log, TEXT("EXIT Agame_mode CTOR"));
}

void Agame_mode::BeginPlay()
{
    UE_LOG(LogTemp, Log, TEXT("ENTER Agame_mode::BeginPlay()"));
    Super::BeginPlay();

    if (Ugame_instance::get()->game_kind() == game_kind::sp)
        start_sp_game();
    else
        start_mp_game();
    UE_LOG(LogTemp, Log, TEXT("EXIT Agame_mode::BeginPlay()"));
}

void Agame_mode::multicast_quit_to_menu_Implementation()
{
    Ugame_instance::get()->load_start_level();
}

Aplaying_hud * Agame_mode::hud() const
{
    if (auto * const pc = GetWorld()->GetFirstPlayerController())
        return Cast<Aplaying_hud>(pc->GetHUD());
    return nullptr;
}

void Agame_mode::start_sp_game()
{
    std::filesystem::path load_path = Ugame_instance::get()->game_to_load();
    if (load_path.empty()) {
        cast(GameState)->play_state_ = play_state::setup;
        cast(GameState)->play_state_changed();
        if (auto * hud_ptr = hud())
            hud_ptr->show_game_setup();
    } else {
        // TODO game_data_.load(load_path);
    }
}

void Agame_mode::start_mp_game()
{
    // TODO
}
