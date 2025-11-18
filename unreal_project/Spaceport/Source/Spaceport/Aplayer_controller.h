#pragma once

#include "constants.hpp"

#include <Aplayer_controller_base.h>
#include <GameFramework/PlayerController.h>
#include "Aplayer_controller.generated.h"


class UInputMappingContext;
class UInputAction;
class Agame_mode;

UCLASS()
class Aplayer_controller : public Aplayer_controller_base
{
    GENERATED_BODY()

public:
    Aplayer_controller();

    void BeginPlay() override;

    UFUNCTION(Server, Reliable)
    void server_quit_to_menu();
    void server_quit_to_menu_Implementation();

    UFUNCTION(Server, Reliable)
    void server_start_game(TArray<uint8> const & params);
    void server_start_game_Implementation(TArray<uint8> const & params);

    int nation_id() const { return nation_id_; }

private:
    int nation_id_ = nation_none;

    friend Agame_mode;
};
