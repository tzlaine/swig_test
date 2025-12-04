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

    UFUNCTION(Server, Reliable)
    void server_save_game(FString const & filename);
    void server_save_game_Implementation(FString const & filename);

    void send_initial_game_state_to_client(TArray<uint8> const & state);
    UFUNCTION(Client, Reliable)
    void client_recv_initial_game_state(TArray<uint8> const & state);
    void
    client_recv_initial_game_state_Implementation(TArray<uint8> const & state);

    void send_day_updates_to_client(TArray<uint8> const & state);
    UFUNCTION(Client, Reliable)
    void client_recv_day_updates(TArray<uint8> const & state);
    void client_recv_day_updates_Implementation(TArray<uint8> const & state);

    void send_month_updates_to_client(TArray<uint8> const & state);
    UFUNCTION(Client, Reliable)
    void client_recv_month_updates(TArray<uint8> const & state);
    void client_recv_month_updates_Implementation(TArray<uint8> const & state);

    void send_year_updates_to_client(TArray<uint8> const & state);
    UFUNCTION(Client, Reliable)
    void client_recv_year_updates(TArray<uint8> const & state);
    void client_recv_year_updates_Implementation(TArray<uint8> const & state);

    UFUNCTION(Server, Reliable)
    void server_toggle_pause();
    void server_toggle_pause_Implementation();

    UFUNCTION(Server, Reliable)
    void server_change_play_speed(int speed);
    void server_change_play_speed_Implementation(int speed);

    int nation_id() const { return nation_id_; }

private:
    int nation_id_ = nation_none;

    friend Agame_mode;
};
