#include "Agame_state.h"
#include "Amap_hud.h"
#include "Aplayer_controller.h"

#include<Net/UnrealNetwork.h>


void Agame_state::play_state_changed()
{
    UE_LOG(LogTemp, Warning, TEXT("Client: play state changed to %s"),
           *UEnum::GetValueAsString(play_state_));
}

void Agame_state::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty> & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(Agame_state, play_state_);
}
