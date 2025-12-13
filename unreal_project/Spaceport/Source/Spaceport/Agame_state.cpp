#include "Agame_state.h"
#include "Ahud_base.h"
#include "utility.hpp"

#include <Engine/World.h>
#include <Net/UnrealNetwork.h>


Agame_state::Agame_state()
{
    bReplicates = true;
    bAlwaysRelevant = true;
    bOnlyRelevantToOwner = false;
}

bool Agame_state::playing_or_paused() const
{
    return ::playing_or_paused(play_state_);
}

void Agame_state::saves_changed()
{
    UE_LOG(
        LogTemp,
        Warning,
        TEXT("List of saves changed to: %s"),
        *FString::Join(saves_, TEXT(", ")));

    if (auto * const hud = hud_base())
        hud->saves_list(saves_);
}

void Agame_state::save_file_changes_changed()
{
    UE_LOG(LogTemp, Warning, TEXT("List of save file changes changed"));

    if (auto * const hud = hud_base())
        hud->saves_changed(save_file_changes_);
}

void Agame_state::play_state_changed()
{
    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Client: play state changed, %s -> %s"),
        *UEnum::GetValueAsString(prev_play_state_),
        *UEnum::GetValueAsString(play_state_));
}

void Agame_state::play_speed_changed()
{
    UE_LOG(LogTemp, Warning, TEXT("Client: play speed=%d"), (int)play_speed_);
}

void Agame_state::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty> & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(Agame_state, saves_);
    DOREPLIFETIME(Agame_state, save_file_changes_);
    DOREPLIFETIME(Agame_state, prev_play_state_);
    DOREPLIFETIME(Agame_state, play_state_);
    DOREPLIFETIME(Agame_state, play_speed_);
}
