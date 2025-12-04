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

Aplayer_controller::Aplayer_controller() {}

void Aplayer_controller::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Log, TEXT("ENTER Aplayer_controller::BeginPlay()"));
    UE_LOG(LogTemp, Log, TEXT("EXIT Aplayer_controller::BeginPlay()"));
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
                // TODO AActor* HitActor = Cast<Afleet_actor>(hit_result.GetActor());
            } else if (GetHitResultUnderCursor(star_channel, false, hit_result)) {
                // TODO AActor* HitActor = Cast<Astar_actor>(hit_result.GetActor());
            } else if (GetHitResultUnderCursor(hex_channel, false, hit_result)) {
                // TODO AActor* HitActor = Cast<Ahex_actor>(hit_result.GetActor());
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
