#pragma once

#include <vector>

#include <CoreMinimal.h>
#include <GameFramework/GameModeBase.h>
#include "Agame_mode_base.generated.h"


struct Ffile_change;

UCLASS()
class Agame_mode_base : public AGameModeBase
{
    GENERATED_BODY()

public:
    Agame_mode_base(FObjectInitializer const & init);

    void PostLogin(APlayerController * player) override;
    void BeginPlay() override;
    void EndPlay(EEndPlayReason::Type reason) override;

    UFUNCTION(NetMulticast, Reliable)
    void multicast_load_playing();
    void multicast_load_playing_Implementation();

    UFUNCTION(NetMulticast, Reliable)
    void load_and_start_newest_game();
    void load_and_start_game_Implementation();

    UFUNCTION(NetMulticast, Reliable)
    void load_and_start_game(FString const & filename);
    void load_and_start_game_Implementation(FString const & filename);

    void publish_save_files();

private:
    void saves_dir_changed(std::vector<Ffile_change> changes);

    int players_ = 0;
};
