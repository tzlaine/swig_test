#include "Agame_state.h"
#include "Amap_hud.h"
#include "Aplayer_controller.h"

#include <Net/UnrealNetwork.h>


void Agame_state::play_state_changed()
{
    UE_LOG(LogTemp, Warning, TEXT("Client: play state changed to %s"),
           *UEnum::GetValueAsString(play_state_));
    // TODO if (play_state_ == play_state::generating) Show "Generating Galaxy..." with progress bar.
}

void Agame_state::generation_percent_changed()
{
    UE_LOG(LogTemp, Warning, TEXT("Client: galaxy generation at %d pct"),
           generation_percent_complete_);
    // TODO: Update progress bar
}

void Agame_state::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty> & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(Agame_state, play_state_);
}
