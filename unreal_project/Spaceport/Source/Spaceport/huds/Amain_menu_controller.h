#pragma once

#include <CoreMinimal.h>
#include <Engine/Classes/GameFramework/PlayerController.h>
#include "Amain_menu_controller.generated.h"


UCLASS()
class Amain_menu_controller : public APlayerController
{
    GENERATED_BODY()

public:
    void BeginPlay();

    UFUNCTION(Server, Reliable)
    void Server_new_sp_game();
    void Server_new_sp_game_Implementation();
};
