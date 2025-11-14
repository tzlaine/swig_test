#include "Amain_menu_game_state.h"
#include "Amain_menu_hud.h"
#include "Smain_menu.h"

#include <Net/UnrealNetwork.h>


Amain_menu_game_state::Amain_menu_game_state()
{
    bReplicates = true;
    bAlwaysRelevant = true;
    bOnlyRelevantToOwner = false;
}

void Amain_menu_game_state::saves_changed()
{
    UE_LOG(LogTemp, Warning, TEXT("List of saves changed to: %s"),
           *FString::Join(saves_, TEXT(", ")));

    if (HasAuthority())
        return;

    auto * const hud =
        GetWorld()->GetFirstPlayerController()->GetHUD<Amain_menu_hud>();
    hud->saves_available(!saves_.IsEmpty());

    // TODO: Notify save/load dialog.
}

void Amain_menu_game_state::save_file_changes_changed()
{
    UE_LOG(LogTemp, Warning, TEXT("List of save file changes changed"));
    // TODO: Notify save/load dialog.
}

void Amain_menu_game_state::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty> & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(Amain_menu_game_state, saves_);
    DOREPLIFETIME(Amain_menu_game_state, save_file_changes_);
}
