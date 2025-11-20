#pragma once

#include "Agame_state_base.h"

#include <CoreMinimal.h>
#include <GameFramework/GameStateBase.h>
#include "Agame_state.generated.h"


UCLASS()
class Agame_state : public Agame_state_base
{
    GENERATED_BODY()

public:
    Agame_state();

    UPROPERTY(ReplicatedUsing = play_state_changed, BlueprintReadOnly,
              Category = "Game State")
    play_state play_state_ = play_state::setup;
    UFUNCTION()
    void play_state_changed();

protected:
    void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty> & props) const override;
};
