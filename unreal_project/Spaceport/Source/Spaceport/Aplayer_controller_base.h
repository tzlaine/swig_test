#pragma once

#include "constants.hpp"

#include <functional>
#include <map>

#include <CoreMinimal.h>
#include <UserSettings/EnhancedInputUserSettings.h>
#include <GameFramework/PlayerController.h>
#include "Aplayer_controller_base.generated.h"


class UMaterialInterface;

struct loaded_material_interfaces
{
    UMaterialInterface * get(FString const & obj_path);

private:
    TMap<FString, TStrongObjectPtr<UMaterialInterface>> materials_;
};

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

    void remap_key(FName name, FKey key);
    void save_user_input_mappings();
    loaded_material_interfaces & materials() { return materials_; }

protected:
    UPROPERTY(EditAnywhere, Category = "Input")
    TSoftObjectPtr<UInputMappingContext> input_mapping_ctx_;
    UPROPERTY(EditAnywhere, Category = "Input")
    TSoftObjectPtr<UInputAction> menu_toggle_action_;

private:
    loaded_material_interfaces materials_;
};
