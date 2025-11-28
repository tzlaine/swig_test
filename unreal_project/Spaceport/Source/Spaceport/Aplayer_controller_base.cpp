#include "Aplayer_controller_base.h"
#include "Ahud_base.h"
#include "Agame_mode_base.h"
#include "utility.hpp"

#include <UserSettings/EnhancedInputUserSettings.h> // TODO

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
            if (imc) {
                if (UEnhancedInputUserSettings * user_settings =
                        input_sys->GetUserSettings()) {
                    user_settings->RegisterInputMappingContext(imc);
#if 0 // TODO: Do this when the user clicks "Apply" in the applicable settings panel.
                    user_settings->SaveSettings();
#endif
                }
                FModifyContextOptions options = {};
                options.bNotifyUserSettings = true;
                input_sys->AddMappingContext(imc, 0, options);
            }
        }
    }

#if 0 // TODO: Use this to populate the applicable settings panel.  Use the code here: https://dev.epicgames.com/community/learning/tutorials/Vp69/unreal-engine-player-mappable-keys-using-enhanced-input to actually do the mapping.
    if (auto * local_player = GetLocalPlayer()) {
        if (auto * subsystem =
                ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
                    local_player)) {

            // print out mappings
            if (UEnhancedInputUserSettings * user_settings =
                    subsystem->GetUserSettings()) {
                if (auto * profile = user_settings->GetActiveKeyProfile()) {
                    auto const & rows = profile->GetPlayerMappingRows();
                    for (auto && [name, row] : rows) {
                        UE_LOG(LogTemp, Warning, TEXT("Active: %s"), *name.ToString());
                        for (auto && mapping : row.Mappings) {
                            UE_LOG(
                                LogTemp,
                                Log,
                                TEXT("%s"),
                                *mapping.GetCurrentKey()
                                     .GetDisplayName()
                                     .ToString());
                        }
                    }
                }
            }
        }
    }
#endif
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

void Aplayer_controller_base::remap_key(FName name, FKey key)
{
    if (auto * local_player = GetLocalPlayer()) {
        if (auto * subsystem =
                ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
                    local_player)) {

            // print out mappings
            if (UEnhancedInputUserSettings * user_settings =
                    subsystem->GetUserSettings()) {
                FMapPlayerKeyArgs args = {};
                args.MappingName = name;
                args.Slot = EPlayerMappableKeySlot::First;
                args.NewKey = key;
                FGameplayTagContainer failure;
                user_settings->MapPlayerKey(args, failure);
            }
        }
    }
}
