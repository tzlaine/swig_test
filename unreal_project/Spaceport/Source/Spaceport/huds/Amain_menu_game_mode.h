#pragma once

#include "Agame_mode_base.h"
#include "dir_watcher.h"

#include <CoreMinimal.h>
#include <GameFramework/GameModeBase.h>
#include "Amain_menu_game_mode.generated.h"


UCLASS()
class Amain_menu_game_mode : public Agame_mode_base
{
    GENERATED_BODY()

public:
    Amain_menu_game_mode(FObjectInitializer const & init);

    void BeginPlay() override;
    void EndPlay(EEndPlayReason::Type reason) override;

    UFUNCTION(NetMulticast, Reliable)
    void load_and_start_newest_game();
    void load_and_start_game_Implementation();

private:
    void saves_dir_changed(std::vector<Ffile_change> changes);
};
