#pragma once

#include <CoreMinimal.h>
#include <GameFramework/PlayerController.h>
#include "Aplayer_controller.generated.h"


UCLASS()
class Aplayer_controller : public APlayerController
{
    GENERATED_BODY()

public:
    void BeginPlay();

    UFUNCTION(Server, Reliable)
    void server_quit_to_menu();
    void server_quit_to_menu_Implementation();
};
