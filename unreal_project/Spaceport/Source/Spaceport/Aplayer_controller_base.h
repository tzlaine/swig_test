#pragma once

#include "constants.hpp"

#include <CoreMinimal.h>
#include <GameFramework/PlayerController.h>
#include "Aplayer_controller_base.generated.h"


class UInputMappingContext;
class UInputAction;

UCLASS()
class Aplayer_controller_base : public APlayerController
{
    GENERATED_BODY()

public:
    Aplayer_controller_base();

    void BeginPlay() override;
    void SetupInputComponent() override;

private:
    TSoftObjectPtr<UInputMappingContext> input_mapping_ctx_;
    TSoftObjectPtr<UInputAction> menu_toggle_action_;
};
