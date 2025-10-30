#pragma once

#include <CoreMinimal.h>
#include <Engine/Classes/GameFramework/PlayerController.h>
#include "Aplayer_controller.generated.h"


UCLASS()
class Aplayer_controller : public APlayerController
{
    GENERATED_BODY()

public:
    void BeginPlay();
};
