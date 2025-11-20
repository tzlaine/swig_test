#include "Agame_state_base.h"
#include "Ahud_base.h"

#include <Net/UnrealNetwork.h>


Agame_state_base::Agame_state_base()
{
    bReplicates = true;
    bAlwaysRelevant = true;
    bOnlyRelevantToOwner = false;
}

void Agame_state_base::play_state_changed()
{
    UE_LOG(LogTemp, Warning, TEXT("Client: play state changed to %s"),
           *UEnum::GetValueAsString(play_state_));
}

void Agame_state_base::saves_changed()
{
    UE_LOG(LogTemp, Warning, TEXT("List of saves changed to: %s"),
           *FString::Join(saves_, TEXT(", ")));

    if (auto * pc = GetWorld()->GetFirstPlayerController()) {
        if (auto * const hud = pc->GetHUD<Ahud_base>()) {
            hud->saves_list(saves_);
        }
    }
}

void Agame_state_base::save_file_changes_changed()
{
    UE_LOG(LogTemp, Warning, TEXT("List of save file changes changed"));

    if (auto * pc = GetWorld()->GetFirstPlayerController()) {
        if (auto * const hud = pc->GetHUD<Ahud_base>()) {
            hud->saves_changed(save_file_changes_);
        }
    }
}

void Agame_state_base::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty> & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(Agame_state_base, play_state_);
    DOREPLIFETIME(Agame_state_base, saves_);
    DOREPLIFETIME(Agame_state_base, save_file_changes_);
}
