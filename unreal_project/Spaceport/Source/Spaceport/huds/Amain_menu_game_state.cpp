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

    if (auto * pc = GetWorld()->GetFirstPlayerController()) {
        if (auto * const hud = pc->GetHUD<Amain_menu_hud>()) {
            hud->saves_list(saves_);
        }
    }
}

void Amain_menu_game_state::save_file_changes_changed()
{
    UE_LOG(LogTemp, Warning, TEXT("List of save file changes changed"));

    if (auto * pc = GetWorld()->GetFirstPlayerController()) {
        if (auto * const hud = pc->GetHUD<Amain_menu_hud>()) {
            hud->saves_changed(save_file_changes_);
        }
    }
}

void Amain_menu_game_state::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty> & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(Amain_menu_game_state, saves_);
    DOREPLIFETIME(Amain_menu_game_state, save_file_changes_);
}
