#pragma once

#include "constants.hpp"

#include <CoreMinimal.h>
#include <GameFramework/PlayerController.h>
#include "Aplayer_controller.generated.h"


class UInputMappingContext;
class UInputAction;
class Agame_mode;

UCLASS()
class Aplayer_controller : public APlayerController
{
    GENERATED_BODY()

public:
    Aplayer_controller();

    void BeginPlay() override;
    void SetupInputComponent() override;

    UFUNCTION(Server, Reliable)
    void server_quit_to_menu();
    void server_quit_to_menu_Implementation();

    UFUNCTION(Server, Reliable)
    void server_start_game(TArray<uint8> const & params);
    void server_start_game_Implementation(TArray<uint8> const & params);

    int nation_id() const { return nation_id_; }

private:
    int nation_id_ = nation_none;

    TSoftObjectPtr<UInputMappingContext> input_mapping_ctx_;
    TSoftObjectPtr<UInputAction> menu_toggle_action_;

    friend Agame_mode;
};
