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
    void Server_new_game(game_kind kind, FFilePath const & save);
    void Server_new_game_Implementation(game_kind kind, FFilePath const & save);

    // TODO UFUNCTION(Server, Reliable)
    // TODO void Server_list_saves();
    // TODO void Server_list_saves_Implementation();
};
