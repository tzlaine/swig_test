#pragma once

#include "constants.hpp"

#include <adobe/name.hpp>

#include <functional>
#include <map>

#include <CoreMinimal.h>
#include <UserSettings/EnhancedInputUserSettings.h>
#include <GameFramework/PlayerController.h>
#include "Aplayer_controller_base.generated.h"


class UMaterialInterface;
class UInputMappingContext;
class UInputAction;
class Uui_defaults_t;
class Umaterials_t;
class Uaudio_cues_t;

UCLASS()
class Aplayer_controller_base : public APlayerController
{
    GENERATED_BODY()

public:
    Aplayer_controller_base();

    void BeginPlay() override;
    void SetupInputComponent() override;

    UFUNCTION(Server, Reliable)
    void server_req_save_files();
    void server_req_save_files_Implementation();

    UFUNCTION(Server, Reliable)
    void server_load_game(FString const & filename);
    void server_load_game_Implementation(FString const & filename);

    UInputMappingContext const & input_mapping_context() const;
    TArray<FEnhancedActionKeyMapping>
    player_mappable_action_key_mappings() const;
    TMap<FKey, FKey> current_to_default_keys() const;

    Uui_defaults_t const & ui_defaults();
    Umaterials_t const & materials();
    Uaudio_cues_t const & audio_cues();
    void remap_key(FName name, FKey key);
    void save_user_input_mappings();

private:
    UPROPERTY(
        EditAnywhere, Category = "UI", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<Uui_defaults_t> ui_defaults_class_;
    UPROPERTY(
        EditAnywhere,
        Category = "Materials",
        meta = (AllowPrivateAccess = "true"))
    TSubclassOf<Umaterials_t> materials_class_;
    UPROPERTY(
        EditAnywhere,
        Category = "Audio cues",
        meta = (AllowPrivateAccess = "true"))
    TSubclassOf<Uaudio_cues_t> audio_cues_class_;

    UPROPERTY()
    TObjectPtr<Uui_defaults_t> ui_defaults_;
    UPROPERTY()
    TObjectPtr<Umaterials_t> materials_;
    UPROPERTY()
    TObjectPtr<Uaudio_cues_t> audio_cues_;

    UPROPERTY(
        EditAnywhere, Category = "Input", meta = (AllowPrivateAccess = "true"))
    TSoftObjectPtr<UInputMappingContext> input_mapping_ctx_;
    UPROPERTY(
        EditAnywhere, Category = "Input", meta = (AllowPrivateAccess = "true"))
    TSoftObjectPtr<UInputAction> menu_toggle_action_;
};
