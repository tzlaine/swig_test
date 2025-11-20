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

Aplayer_controller::Aplayer_controller()
{}

void Aplayer_controller::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Log, TEXT("ENTER Aplayer_controller::BeginPlay()"));
    UE_LOG(LogTemp, Log, TEXT("EXIT Aplayer_controller::BeginPlay()"));
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
        hud->hide_game_setup();

    auto * gm = GetWorld()->GetAuthGameMode<Agame_mode>();
    if (!gm)
        return;

    gm->distribute_initial_game_state(params);
}

void Aplayer_controller::server_save_game_Implementation(
    FString const & filename)
{
    auto * gm = GetWorld()->GetAuthGameMode<Agame_mode>();
    if (!gm)
        return;

    gm->save_game(filename);
}
