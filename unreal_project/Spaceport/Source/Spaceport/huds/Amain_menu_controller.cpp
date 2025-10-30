#include "Amain_menu_controller.h"
#include "Amain_menu_game_mode.h"
#include "game_instance.h"


void Amain_menu_controller::BeginPlay()
{
    Super::BeginPlay();

    FInputModeUIOnly InputModeData;
    InputModeData.SetLockMouseToViewportBehavior(
        EMouseLockMode::DoNotLock);
    SetInputMode(InputModeData);
    SetShowMouseCursor(true);
}

void Amain_menu_controller::Server_new_game_Implementation(
    game_kind kind, FFilePath const & save)
{
    Amain_menu_game_mode * gm =
        GetWorld()->GetAuthGameMode<Amain_menu_game_mode>();
    if (!gm)
        return;
    Ugame_instance::get()->game_kind(kind);
    Ugame_instance::get()->game_to_load(save);
    gm->Multicast_new_game();
}
