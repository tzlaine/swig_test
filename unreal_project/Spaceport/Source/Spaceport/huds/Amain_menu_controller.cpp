#include "Amain_menu_controller.h"
#include "Amain_menu_game_mode.h"


void Amain_menu_controller::BeginPlay()
{
    Super::BeginPlay();

    FInputModeUIOnly InputModeData;
    InputModeData.SetLockMouseToViewportBehavior(
        EMouseLockMode::DoNotLock);
    SetInputMode(InputModeData);
    SetShowMouseCursor(true);
}

void Amain_menu_controller::Server_new_sp_game_Implementation()
{
    Amain_menu_game_mode * gm =
        GetWorld()->GetAuthGameMode<Amain_menu_game_mode>();
    if (!gm)
        return;
    gm->Multicast_new_sp_game();
}
