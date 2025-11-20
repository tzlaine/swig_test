#pragma once

#include "model.hpp"

#include <CoreMinimal.h>
#include <GameFramework/GameModeBase.h>
#include "Agame_mode_base.generated.h"


UCLASS()
class Agame_mode_base : public AGameModeBase
{
    GENERATED_BODY()

public:
    Agame_mode_base(FObjectInitializer const & init);

    UFUNCTION(NetMulticast, Reliable)
    void multicast_load_playing();
    void multicast_load_playing_Implementation();

    UFUNCTION(NetMulticast, Reliable)
    void load_and_start_game(FString const & filename);
    void load_and_start_game_Implementation(FString const & filename);

protected:
    model model_;
};
