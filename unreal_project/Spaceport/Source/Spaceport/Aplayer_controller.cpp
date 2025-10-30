#include "Aplayer_controller.h"


void Aplayer_controller::BeginPlay()
{
    Super::BeginPlay();

    FInputModeUIOnly InputModeData;
    InputModeData.SetLockMouseToViewportBehavior(
        EMouseLockMode::DoNotLock);
    SetInputMode(InputModeData);
    SetShowMouseCursor(true);
}
