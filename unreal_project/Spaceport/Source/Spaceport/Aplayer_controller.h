#pragma once

#include "Amap_pawn_base_fwd.hpp"
#include "constants.hpp"

#include <span>

#include <Aplayer_controller_base.h>
#include <GameFramework/PlayerController.h>
#include "Aplayer_controller.generated.h"


class Amap_pawn_base;
class Agame_mode;
struct FInputActionValue;
class UInputAction;

UCLASS()
class Aplayer_controller : public Aplayer_controller_base
{
    GENERATED_BODY()

public:
    Aplayer_controller();

    void BeginPlay() override;
    void Tick(float delta) override;
    void SetupInputComponent() override;

    UFUNCTION(Server, Reliable)
    void server_quit_to_menu();
    void server_quit_to_menu_Implementation();

    UFUNCTION(Server, Reliable)
    void server_start_game(TArray<uint8> const & params);
    void server_start_game_Implementation(TArray<uint8> const & params);

    UFUNCTION(Server, Reliable)
    void server_save_game(FString const & filename);
    void server_save_game_Implementation(FString const & filename);

    UFUNCTION(Client, Reliable)
    void
    client_recv_initial_game_state(int nation_id, TArray<uint8> const & state);
    void client_recv_initial_game_state_Implementation(
        int nation_id, TArray<uint8> const & state);

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
    enum struct deselect { no, yes };

    void dehover_all();
    void deselect_all();
    void hover(Amap_pawn_base * pawn);
    void select(Amap_pawn_base * pawn, deselect deselect_curr);
    void select(
        std::span<Amap_pawn_base *> pawns,
        deselect deselect_curr,
        map_pawn_kind kind);
    void select_in_box(map_pawn_kind selecting, deselect deselect_curr);

    int nation_id_ = nation_none;
    std::vector<Amap_pawn_base *> curr_hovers_;
    std::vector<Amap_pawn_base *> curr_selections_;
    bool keep_selected_key_down_ = false;
    bool alternate_selection_key_down_ = false;
    FVector2D selection_box_first_;
    FVector2D selection_box_last_;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Input",
        meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> select_object_action_;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Input",
        meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> order_selected_action_;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Input",
        meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> incr_play_speed_action_;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Input",
        meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> decr_play_speed_action_;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Input",
        meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> pause_toggle_action_;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Input",
        meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> keep_selected_action_;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Input",
        meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> alternate_selection_action_;

    friend Agame_mode;
};
