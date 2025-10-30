#pragma once

#include <CoreMinimal.h>
#include <GameFramework/GameModeBase.h>
#include "Amain_menu_game_mode.generated.h"


UCLASS()
class Amain_menu_game_mode : public AGameModeBase
{
    GENERATED_BODY()

public:
    Amain_menu_game_mode(FObjectInitializer const & init);

    virtual void BeginPlay() override;

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_new_sp_game();
    void Multicast_new_sp_game_Implementation();
};
