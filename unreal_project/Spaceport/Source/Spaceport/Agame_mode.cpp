#include "Agame_mode.h"
#include "Agame_state.h"
#include "Amap_hud.h"
#include "Aplayer_controller.h"


Agame_mode::Agame_mode(FObjectInitializer const & init) :
    AGameModeBase(init)
{
    HUDClass = Amap_hud::StaticClass();
    PlayerControllerClass = Aplayer_controller::StaticClass();
}

void Agame_mode::start_sp_game()
{
    if (Agame_state * gs = GetGameState<Agame_state>()) {
        gs->play_state_ = play_state::setup;
        gs->play_state_changed();
    }
}

void Agame_mode::BeginPlay()
{
    Super::BeginPlay();
}
