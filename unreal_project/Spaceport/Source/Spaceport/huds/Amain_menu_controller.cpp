#include "Amain_menu_controller.h"
#include "Amain_menu_game_mode.h"
#include "Amain_menu_game_state.h"
#include "game_instance.h"


Amain_menu_controller::Amain_menu_controller()
{
    bReplicates = true;
    bAlwaysRelevant = true;
    bOnlyRelevantToOwner = true;
}

void Amain_menu_controller::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Log, TEXT("ENTER Amain_menu_controller::BeginPlay()"));
    FInputModeUIOnly InputModeData;
    InputModeData.SetLockMouseToViewportBehavior(
        EMouseLockMode::DoNotLock);
    SetInputMode(InputModeData);
    SetShowMouseCursor(true);
    UE_LOG(LogTemp, Log, TEXT("EXIT Amain_menu_controller::BeginPlay()"));
}

void Amain_menu_controller::server_new_game_Implementation(
    game_kind kind, FFilePath const & save)
{
    auto * gm = GetWorld()->GetAuthGameMode<Agame_mode_base>();
    if (!gm)
        return;
    Ugame_instance::get()->game_kind(kind);
    Ugame_instance::get()->game_to_load(*save.FilePath);
    gm->multicast_load_playing();
}
