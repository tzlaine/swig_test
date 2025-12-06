#include "Aplayer_controller.h"
#include "Agame_mode.h"
#include "Amap_fleet.h"
#include "Amap_system.h"
#include "Amap_hex.h"
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

Aplayer_controller::Aplayer_controller() {}

void Aplayer_controller::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Log, TEXT("ENTER Aplayer_controller::BeginPlay()"));
    UE_LOG(LogTemp, Log, TEXT("EXIT Aplayer_controller::BeginPlay()"));
}

void Aplayer_controller::Tick(float delta)
{
    Super::Tick(delta);

    FHitResult hit_result;
    if (GetHitResultUnderCursor(fleet_channel, false, hit_result)) {
        Amap_fleet * fleet = Cast<Amap_fleet>(hit_result.GetActor());
        if (fleet)
            hover_one(fleet);
    } else if (GetHitResultUnderCursor(star_channel, false, hit_result)) {
        Amap_system * system = Cast<Amap_system>(hit_result.GetActor());
        if (system)
            hover_one(system);
    } else if (GetHitResultUnderCursor(hex_channel, false, hit_result)) {
        Amap_hex * hex = Cast<Amap_hex>(hit_result.GetActor());
        if (hex)
            hover_one(hex);
    } else {
        dehover_curr();
    }
}

void Aplayer_controller::SetupInputComponent()
{
    Super::SetupInputComponent();

    UEnhancedInputComponent * eic =
        Cast<UEnhancedInputComponent>(InputComponent);
    check(eic);

    eic->BindActionValueLambda(
        select_object_action_, ETriggerEvent::Completed, [this](auto const &) {
            FHitResult hit_result;
            if (GetHitResultUnderCursor(fleet_channel, false, hit_result)) {
                Amap_fleet * fleet = Cast<Amap_fleet>(hit_result.GetActor());
                if (fleet)
                    select_one(fleet);
            } else if (GetHitResultUnderCursor(
                           star_channel, false, hit_result)) {
                Amap_system * system = Cast<Amap_system>(hit_result.GetActor());
                if (system)
                    select_one(system);
            } else if (GetHitResultUnderCursor(
                           hex_channel, false, hit_result)) {
                Amap_hex * hex = Cast<Amap_hex>(hit_result.GetActor());
                if (hex)
                    select_one(hex);
            } else {
                deselect_curr();
            }
        });
    eic->BindActionValueLambda(
        order_selected_action_, ETriggerEvent::Completed, [this](auto const &) {
            // TODO
        });
    eic->BindActionValueLambda(
        pause_toggle_action_, ETriggerEvent::Completed, [this](auto const &) {
            // TODO
        });
}

void Aplayer_controller::server_quit_to_menu_Implementation()
{
    auto * gm = GetWorld()->GetAuthGameMode<Agame_mode>();
    if (!gm)
        return;
    gm->multicast_quit_to_menu();
}

void Aplayer_controller::server_start_game_Implementation(
    TArray<uint8> const & params)
{
    if (auto * hud = cast(GetHUD()))
        hud->remove_game_setup_widget();

    auto * gm = GetWorld()->GetAuthGameMode<Agame_mode>();
    if (!gm)
        return;
    gm->setup_for_game_start(params);
}

void Aplayer_controller::server_save_game_Implementation(
    FString const & filename)
{
    auto * gm = GetWorld()->GetAuthGameMode<Agame_mode>();
    if (!gm)
        return;
    gm->save_game(filename);
}

void Aplayer_controller::send_initial_game_state_to_client(
    TArray<uint8> const & state)
{
    if (HasAuthority())
        client_recv_initial_game_state(state);
}

void Aplayer_controller::client_recv_initial_game_state_Implementation(
    TArray<uint8> const & state)
{
    if (HasAuthority())
        return;
    // TODO
}

void Aplayer_controller::send_day_updates_to_client(TArray<uint8> const & state)
{
    if (HasAuthority())
        client_recv_day_updates(state);
}

void Aplayer_controller::client_recv_day_updates_Implementation(
    TArray<uint8> const & state)
{
    if (HasAuthority())
        return;
    // TODO
}

void Aplayer_controller::send_month_updates_to_client(
    TArray<uint8> const & state)
{
    if (HasAuthority())
        client_recv_month_updates(state);
}

void Aplayer_controller::client_recv_month_updates_Implementation(
    TArray<uint8> const & state)
{
    if (HasAuthority())
        return;
    // TODO
}

void Aplayer_controller::send_year_updates_to_client(TArray<uint8> const & state)
{
    if (HasAuthority())
        client_recv_year_updates(state);
}

void Aplayer_controller::client_recv_year_updates_Implementation(
    TArray<uint8> const & state)
{
    if (HasAuthority())
        return;
    // TODO
}

void Aplayer_controller::server_toggle_pause_Implementation()
{
    auto * gm = GetWorld()->GetAuthGameMode<Agame_mode>();
    if (!gm)
        return;
    gm->toggle_pause();
}

void Aplayer_controller::server_change_play_speed_Implementation(int speed)
{
    auto * gm = GetWorld()->GetAuthGameMode<Agame_mode>();
    if (!gm)
        return;
    gm->play_speed(speed);
}

void Aplayer_controller::dehover_curr()
{
    for (auto * p : curr_hovers_) {
        p->hover(false);
    }
    curr_hovers_.clear();
}

void Aplayer_controller::deselect_curr()
{
    for (auto * p : curr_selections_) {
        p->select(false);
    }
    curr_selections_.clear();
}

void Aplayer_controller::hover_one(Amap_pawn_base * pawn)
{
    dehover_curr();
    if (std::ranges::any_of(
            curr_selections_, [pawn](auto * e) { return e == pawn; })) {
        return;
    }
    pawn->hover(true);
    curr_hovers_.push_back(pawn);
}

void Aplayer_controller::select_one(Amap_pawn_base * pawn)
{
    deselect_curr();
    pawn->select(true);
    curr_selections_.push_back(pawn);
}
