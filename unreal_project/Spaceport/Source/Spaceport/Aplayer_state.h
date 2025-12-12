#pragma once

#include <CoreMinimal.h>
#include <GameFramework/PlayerState.h>
#include "Aplayer_state.generated.h"


UCLASS()
class Aplayer_state : public APlayerState
{
    GENERATED_BODY()

public:
    Aplayer_state();

    int player_id() const;

    void player_id(int id);

protected:
    void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty> & props) const override;

    UPROPERTY(Replicated)
    int player_id_ = 0;
};
