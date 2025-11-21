#include "Aplayer_controller_base.h"
#include "Ahud_base.h"
#include "Agame_mode_base.h"
#include "utility.hpp"

#include <EnhancedInputComponent.h>
#include <EnhancedInputSubsystems.h>
#include <InputAction.h>
#include <InputMappingContext.h>
#include <Engine/LocalPlayer.h>
#include <Engine/World.h>


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
        if (auto * input_sys =
            local_player->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()) {
            if (imc)
                input_sys->AddMappingContext(imc, 0);
        }
    }
}

void Aplayer_controller_base::SetupInputComponent()
{
    Super::SetupInputComponent();

    EnableInput(this);

    if (auto * eic = Cast<UEnhancedInputComponent>(InputComponent)) {
        UInputAction * menu_toggle = menu_toggle_action_.LoadSynchronous();
        if (menu_toggle) {
            eic->BindActionInstanceLambda(
                menu_toggle, ETriggerEvent::Started, [this](auto const &) {
                    if (auto * hud = hud_base(GetHUD()))
                        hud->escape_pressed();
                });
        }
    }
}

void Aplayer_controller_base::server_req_save_files_Implementation()
{
    auto * gm = GetWorld()->GetAuthGameMode<Agame_mode_base>();
    if (!gm)
        return;

    gm->publish_save_files();
}

void Aplayer_controller_base::server_load_game_Implementation(
    FString const & filename)
{
    auto * gm = GetWorld()->GetAuthGameMode<Agame_mode_base>();
    if (!gm)
        return;

    gm->load_and_start_game(filename);
}
