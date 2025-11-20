#include "Agame_state.h"
#include "Aplayer_controller.h"

#include <Net/UnrealNetwork.h>


Agame_state::Agame_state() {}

void Agame_state::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty> & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
