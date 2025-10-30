#include "Amain_menu_game_mode.h"
#include "Amain_menu_hud.h"
#include "Amain_menu_controller.h"
#include "game_instance.h"

#include <Kismet/GameplayStatics.h>


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

void Amain_menu_game_mode::Multicast_new_game_Implementation()
{
    UGameplayStatics::OpenLevel(::world(), TEXT("playing"), true);
}
