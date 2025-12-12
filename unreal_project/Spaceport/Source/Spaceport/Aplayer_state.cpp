#include "Aplayer_state.h"

#include <Net/UnrealNetwork.h>


Aplayer_state::Aplayer_state() { PrimaryActorTick.bCanEverTick = false; }

int Aplayer_state::player_id() const { return player_id_; }

void Aplayer_state::player_id(int id)
{
    player_id_ = id;
    UE_LOG(LogTemp, Log, TEXT("Aplayer_state: player id=%d"), player_id_);
}

void Aplayer_state::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty> & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(Aplayer_state, player_id_);
}
