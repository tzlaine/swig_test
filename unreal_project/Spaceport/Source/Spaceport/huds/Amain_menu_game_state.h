#pragma once

#include <CoreMinimal.h>
#include <IDirectoryWatcher.h>
#include <GameFramework/GameStateBase.h>
#include "Amain_menu_game_state.generated.h"


USTRUCT(BlueprintType)
struct Freplicable_FFileChangeData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString file;

    UPROPERTY(BlueprintReadOnly)
    uint8 action;

    bool operator==(Freplicable_FFileChangeData const &) const = default;
};

UCLASS()
class Amain_menu_game_state : public AGameStateBase
{
    GENERATED_BODY()

public:
    UFUNCTION()
    void saves_changed();
    UPROPERTY(ReplicatedUsing = saves_changed, BlueprintReadOnly,
              Category = "Game State")
    TArray<FString> saves_;

    UFUNCTION()
    void save_file_changes_changed();
    UPROPERTY(ReplicatedUsing = save_file_changes_changed, BlueprintReadOnly,
              Category = "Game State")
    TArray<Freplicable_FFileChangeData> save_file_changes_;

protected:
    virtual void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty> & props) const override;
};
