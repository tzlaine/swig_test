#pragma once

#include <CoreMinimal.h>
#include <GameFramework/PlayerController.h>
#include "Amain_menu_controller.generated.h"


UCLASS()
class Amain_menu_controller : public APlayerController
{
    GENERATED_BODY()

public:
    Amain_menu_controller();

    void BeginPlay();

    UFUNCTION(Server, Reliable)
    void server_new_game(game_kind kind, FFilePath const & save);
    void server_new_game_Implementation(game_kind kind, FFilePath const & save);
};
