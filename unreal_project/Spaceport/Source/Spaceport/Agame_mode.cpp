#include "Agame_mode.h"
#include "Agame_state.h"
#include "Aplaying_hud.h"
#include "Aplayer_controller.h"
#include "game_instance.h"
#include "utility.hpp"

#include <filesystem>


namespace {
    Agame_state * cast(AGameStateBase * base)
    {
        return Cast<Agame_state>(base);
    }
}

Agame_mode::Agame_mode(FObjectInitializer const & init) : Agame_mode_base(init)
{
    UE_LOG(LogTemp, Log, TEXT("ENTER Agame_mode CTOR"));
    HUDClass = Aplaying_hud::StaticClass();
    PlayerControllerClass = Aplayer_controller::StaticClass();
    GameStateClass = Agame_state::StaticClass();
    UE_LOG(LogTemp, Log, TEXT("EXIT Agame_mode CTOR"));

    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
    bAlwaysRelevant = true;
    bOnlyRelevantToOwner = true;
}

void Agame_mode::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Log, TEXT("ENTER Agame_mode::BeginPlay()"));
    if (Ugame_instance::get()->game_kind() == game_kind::sp)
        ready_for_sp_game();
    else
        ready_for_mp_game();
    UE_LOG(LogTemp, Log, TEXT("EXIT Agame_mode::BeginPlay()"));
}

void Agame_mode::Tick(float secs)
{
    Super::Tick(secs);

    if (cast(GameState)->play_state_ == play_state::generating) {
        int percent_update = 0;
        if (percent_complete_->try_pop(percent_update)) {
            if (auto * hud_ptr = hud())
                hud_ptr->generating_percent_update(percent_update);
        }

        if (generation_complete_) {
            start_play();
            if (auto * hud_ptr = hud())
                hud_ptr->hide_generating_galaxy();
        }
    }
}

void Agame_mode::multicast_quit_to_menu_Implementation()
{
    Ugame_instance::get()->load_start_level();
}

void Agame_mode::distribute_initial_game_state_Implementation(
    TArray<uint8> const & params_)
{
    if (params_.IsEmpty()) {
        start_play();
        return;
    }

    if (auto * hud_ptr = hud())
        hud_ptr->show_generating_galaxy();

    cast(GameState)->play_state_ = play_state::generating;
    cast(GameState)->play_state_changed();
    percent_complete_ = std::make_unique<concurrent_queue<int>>();

    auto params = from_tarray<game_start_params_t>(params_);
    generation_thread_ =
        std::jthread([&, params = std::move(params), this] {
            model_.generate_galaxy(
                params, *percent_complete_, generation_complete_);
        });
}

void Agame_mode::save_game(FString const & filename)
{
    std::filesystem::path path(*filename);
    model_.save(std::move(path));
}

Aplaying_hud * Agame_mode::hud() const // TODO: Remove.
{
    if (auto * const pc = GetWorld()->GetFirstPlayerController())
        return Cast<Aplaying_hud>(pc->GetHUD());
    return nullptr;
}

void Agame_mode::ready_for_sp_game()
{
    std::filesystem::path load_path = Ugame_instance::get()->game_to_load();
    if (load_path.empty()) {
        cast(GameState)->play_state_ = play_state::setup;
        cast(GameState)->play_state_changed();
        if (auto * hud_ptr = hud())
            hud_ptr->show_game_setup();
    } else {
        model_.load(load_path);
        distribute_initial_game_state(TArray<uint8>{});
    }
}

void Agame_mode::ready_for_mp_game()
{
    // TODO
}

void Agame_mode::start_play()
{
    cast(GameState)->play_state_ = play_state::playing;
    cast(GameState)->play_state_changed();

    // TODO: Spawn actors; show map UI (side panel, etc.).
}
