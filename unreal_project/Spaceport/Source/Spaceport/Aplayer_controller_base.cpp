#include "Aplayer_controller_base.h"
// TODO
#include "Aplaying_hud.h"

#include <EnhancedInputComponent.h>
#include <EnhancedInputSubsystems.h>
#include <InputAction.h>
#include <InputMappingContext.h>
#include <Engine/LocalPlayer.h>
#include <Engine/World.h>


namespace {
    // TODO: Also need a hud base, instead of using just this.
    Aplaying_hud * cast(AHUD * base)
    {
        return Cast<Aplaying_hud>(base);
    }
}

Aplayer_controller_base::Aplayer_controller_base() :
    input_mapping_ctx_(FString(TEXT("/Game/ui/input/input_mapping_context.input_mapping_context"))),
    menu_toggle_action_(FString(TEXT("/Game/ui/input/menu_toggle.menu_toggle")))
{
    bReplicates = true;
    bAlwaysRelevant = true;
    bOnlyRelevantToOwner = true;
}

void Aplayer_controller_base::BeginPlay()
{
    Super::BeginPlay();

    FInputModeGameAndUI input_mode;
    input_mode.SetLockMouseToViewportBehavior(
        EMouseLockMode::DoNotLock);
    SetInputMode(input_mode);
    SetShowMouseCursor(true);

    UInputMappingContext * imc = input_mapping_ctx_.LoadSynchronous();

    if (ULocalPlayer * local_player = GetLocalPlayer()) {
        if (UEnhancedInputLocalPlayerSubsystem * input_sys =
                local_player
                    ->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()) {
            if (imc)
                input_sys->AddMappingContext(imc, 0);
        }
    }
}

void Aplayer_controller_base::SetupInputComponent()
{
    Super::SetupInputComponent();

    EnableInput(this);

    if (UEnhancedInputComponent * eic =
            Cast<UEnhancedInputComponent>(InputComponent)) {
        UInputAction * menu_toggle = menu_toggle_action_.LoadSynchronous();
        if (menu_toggle) {
            eic->BindActionInstanceLambda(
                menu_toggle, ETriggerEvent::Started, [this](auto const &) {
                    if (auto * hud = cast(GetHUD()))
                        hud->escape_pressed();
                });
        }
    }
}
