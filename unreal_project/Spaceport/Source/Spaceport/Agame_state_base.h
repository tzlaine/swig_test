#pragma once

#include "Agame_mode.h"
#include "dir_watcher.h"

#include <CoreMinimal.h>
#include <GameFramework/GameStateBase.h>
#include "Agame_state_base.generated.h"


UCLASS()
class Agame_state_base : public AGameStateBase
{
    GENERATED_BODY()

public:
    Agame_state_base();

    UPROPERTY(ReplicatedUsing = play_state_changed, BlueprintReadOnly,
              Category = "Game State")
    play_state play_state_ = play_state::setup;
    UFUNCTION()
    void play_state_changed();

    UFUNCTION()
    void saves_changed();
    UPROPERTY(ReplicatedUsing = saves_changed, BlueprintReadOnly,
              Category = "Game State")
    TArray<FString> saves_;

    UFUNCTION()
    void save_file_changes_changed();
    UPROPERTY(ReplicatedUsing = save_file_changes_changed, BlueprintReadOnly,
              Category = "Game State")
    TArray<Ffile_change> save_file_changes_;

protected:
    void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty> & props) const override;
};
