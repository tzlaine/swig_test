#pragma once

#include "Agame_mode.h"

#include <CoreMinimal.h>
#include <GameFramework/GameStateBase.h>
#include "Agame_state.generated.h"


UCLASS()
class Agame_state : public AGameStateBase
{
    GENERATED_BODY()

public:
    UPROPERTY(ReplicatedUsing = play_state_changed, BlueprintReadOnly,
              Category = "Game State")
    play_state play_state_ = play_state::setup;
    UFUNCTION()
    void play_state_changed();

    UPROPERTY(ReplicatedUsing = generation_percent_changed, BlueprintReadOnly,
              Category = "Game State")
    int generation_percent_complete_ = 0;
    UFUNCTION()
    void generation_percent_changed();

protected:
    virtual void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty> & props) const override;
};
