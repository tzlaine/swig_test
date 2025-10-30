#include "Amain_menu_game_mode.h"
#include "Amain_menu_hud.h"
#include "Amain_menu_controller.h"


Amain_menu_game_mode::Amain_menu_game_mode(FObjectInitializer const & init) :
    AGameModeBase(init)
{
    HUDClass = Amain_menu_hud::StaticClass();
    PlayerControllerClass = Amain_menu_controller::StaticClass();
}

void Amain_menu_game_mode::BeginPlay()
{
    Super::BeginPlay();
}
