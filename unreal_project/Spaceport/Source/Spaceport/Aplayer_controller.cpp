#include "Aplayer_controller.h"
#include "Agame_mode.h"
#include "Aplaying_hud.h"

#include <EnhancedInputComponent.h>
#include <EnhancedInputSubsystems.h>
#include <InputAction.h>
#include <InputMappingContext.h>
#include <Engine/LocalPlayer.h>
#include <Engine/World.h>


namespace {
    Aplaying_hud * cast(AHUD * base)
    {
        return Cast<Aplaying_hud>(base);
    }
}

Aplayer_controller::Aplayer_controller() :
    input_mapping_ctx_(FString(TEXT("/Game/ui/input/input_mapping_context.input_mapping_context"))),
    menu_toggle_action_(FString(TEXT("/Game/ui/input/menu_toggle.menu_toggle")))
{
    bReplicates = true;
    bAlwaysRelevant = true;
    bOnlyRelevantToOwner = true;
}

void Aplayer_controller::BeginPlay()
{
    UE_LOG(LogTemp, Log, TEXT("ENTER Aplayer_controller::BeginPlay()"));
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

    UE_LOG(LogTemp, Log, TEXT("EXIT Aplayer_controller::BeginPlay()"));
}

void Aplayer_controller::SetupInputComponent()
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
                        hud->toggle_main_menu();
                });
        }
    }
}

void Aplayer_controller::server_quit_to_menu_Implementation()
{
    Agame_mode * gm = GetWorld()->GetAuthGameMode<Agame_mode>();
    if (!gm)
        return;
    gm->multicast_quit_to_menu();
}

void Aplayer_controller::server_start_game_Implementation(
    TArray<uint8> const & params)
{
    if (auto * hud = cast(GetHUD()))
        hud->hide_game_setup();

    Agame_mode * gm = GetWorld()->GetAuthGameMode<Agame_mode>();
    if (!gm)
        return;

    gm->distribute_initial_game_state(params);
}
