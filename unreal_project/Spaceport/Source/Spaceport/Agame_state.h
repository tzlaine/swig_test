#pragma once

#include "Agame_mode_fwd.h"
#include "dir_watcher.h"

#include <CoreMinimal.h>
#include <GameFramework/GameStateBase.h>
#include "Agame_state.generated.h"


UCLASS()
class Agame_state : public AGameStateBase
{
    GENERATED_BODY()

public:
    Agame_state();

    bool playing_or_paused() const;

    UPROPERTY(
        ReplicatedUsing = saves_changed,
        BlueprintReadOnly,
        Category = "Game State")
    TArray<FString> saves_;
    UFUNCTION()
    void saves_changed();

    UPROPERTY(
        ReplicatedUsing = save_file_changes_changed,
        BlueprintReadOnly,
        Category = "Game State")
    TArray<Ffile_change> save_file_changes_;
    UFUNCTION()
    void save_file_changes_changed();

    UPROPERTY(ReplicatedUsing = play_state_changed)
    play_state prev_play_state_ = play_state::start_menu;
    UPROPERTY(ReplicatedUsing = play_state_changed)
    play_state play_state_ = play_state::start_menu;
    UFUNCTION()
    void play_state_changed();

    UPROPERTY(ReplicatedUsing = play_speed_changed)
    int play_speed_ = 1;
    UFUNCTION()
    void play_speed_changed();

protected:
    void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty> & props) const override;
};
