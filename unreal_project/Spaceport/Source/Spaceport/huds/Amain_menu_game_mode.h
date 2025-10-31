#pragma once

#include <CoreMinimal.h>
#include <IDirectoryWatcher.h>
#include <GameFramework/GameModeBase.h>
#include "Amain_menu_game_mode.generated.h"


UCLASS()
class Amain_menu_game_mode : public AGameModeBase
{
    GENERATED_BODY()

public:
    Amain_menu_game_mode(FObjectInitializer const & init);

    virtual void BeginPlay() override;
    virtual void EndPlay(EEndPlayReason::Type reason) override;

    UFUNCTION(NetMulticast, Reliable)
    void multicast_new_game();
    void multicast_new_game_Implementation();

private:
    void watched_dir_changed(TArray<FFileChangeData> const & FileChanges);

    FString watched_dir_;
    IDirectoryWatcher * dir_watcher_ = nullptr;
    FDelegateHandle dir_watcher_handle_;
};
