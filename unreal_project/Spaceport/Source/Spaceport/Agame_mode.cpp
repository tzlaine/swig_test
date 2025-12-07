#include "Agame_mode.h"
#include "Agame_state.h"
#include "Amap_fleet.h"
#include "Amap_hex.h"
#include "Amap_system.h"
#include "Aplaying_hud.h"
#include "Aplayer_controller.h"
#include "game_instance.h"
#include "map_util.hpp"
#include "space_creator_actor_config.hpp"
#include "ui_defaults.h"
#include "utility.hpp"

#include <filesystem>


namespace {
    Agame_state * cast(AGameStateBase * base)
    {
        return Cast<Agame_state>(base);
    }

    float seconds_between_day_ticks(int speed)
    {
        check(1 <= speed && speed <= 5);
        if (speed == 5)
            return 0.0f;
        return 1.0f / speed;
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
            if (auto * hud_ptr = playing_hud())
                hud_ptr->generating_percent_update(percent_update);
        }

        if (generation_complete_) {
            signal_start_of_play();
            if (auto * hud_ptr = playing_hud())
                hud_ptr->remove_generating_widget();
        }
    }

    if (cast(GameState)->play_state_ == play_state::paused)
        return;

    // TODO: Update moving actors on the map; do the next day/month/year tick(s)
    // if necessary; send out model updates to clients.
}

void Agame_mode::multicast_quit_to_menu_Implementation()
{
    Ugame_instance::get()->load(level::start);
}

void Agame_mode::setup_for_game_start_Implementation(
    TArray<uint8> const & params_)
{
    if (params_.IsEmpty()) {
        signal_start_of_play();
        return;
    }

    if (auto * hud_ptr = playing_hud())
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
    auto f = to_path(filename);
    f += TEXT(".sav");
    model_.save(save_dir_path() / f);
}

void Agame_mode::toggle_pause()
{
    if (cast(GameState)->play_state_ != play_state::playing &&
        cast(GameState)->play_state_ != play_state::paused) {
        return;
    }
    if (cast(GameState)->play_state_ == play_state::playing) {
        // TODO: Give a notification of duration of pause in MP.
        cast(GameState)->play_state_ = play_state::paused;
    } else {
        cast(GameState)->play_state_ = play_state::playing;
    }
    cast(GameState)->play_state_changed();
}

void Agame_mode::play_speed(int speed)
{
    if (cast(GameState)->play_state_ != play_state::playing)
        return;
    // TODO: Give a notification of speed change a bit before changing it in
    // MP.
    speed = std::clamp(1, 5, speed);
    model_.set_speed(speed);
    cast(GameState)->play_speed_ = speed;
    cast(GameState)->play_speed_changed();
}

void Agame_mode::ready_for_sp_game()
{
    std::filesystem::path load_path = Ugame_instance::get()->game_to_load();
    if (load_path.empty()) {
        cast(GameState)->play_state_ = play_state::setup;
        cast(GameState)->play_state_changed();
        if (auto * hud_ptr = playing_hud())
            hud_ptr->show_game_setup();
    } else {
        try {
            model_.load(load_path);
            setup_for_game_start(TArray<uint8>{});
        } catch (failed_deserialization const & e) {
            FText message = FText::Format(
                loc_text(TEXT("load_game_failed_message")),
                FText::FromString(FString(e.what())));
            Ugame_instance::get()->defer_notification(
                level::start, TEXT("load_game_failed"), std::move(message));
            multicast_quit_to_menu();
        }
    }
}

void Agame_mode::ready_for_mp_game()
{
    // TODO
}

void Agame_mode::signal_start_of_play()
{
    check(model_.game_state());
    auto const & gs = *model_.game_state();

    // TODO: Starting with just one hex; need to do all of them, of course.
    for (auto const & hex : model_.hexes()) {
        if (hex.province_id == prov_galactic_center) {
            // TODO: Populate with LOTS of non-clickable, non-hoverable stars.
            continue;
        }
        if (hex.province_id == prov_galactic_bulge) {
            // TODO: Populate with non-clickable, non-hoverable stars.
            continue;
        }
        if (hex.province_id == prov_off_map) {
            // TODO: Populate with non-clickable, non-hoverable stars.
            continue;
        }

        auto const hex_xy = hex_position(hex.coord, gs.map_height);
        auto const hex_location = to_fvector(hex_xy) * ui_defaults().map_scale_;
        Amap_hex * hex_pawn = GetWorld()->SpawnActor<Amap_hex>(
            hex_class_, hex_location, FRotator(), FActorSpawnParameters());
        // TODO: Set the hex ID in *hex_pawn.

        for (int i = hex.first_system, last = hex.last_system; i < last; ++i) {
            auto const & system = gs.systems[i];
            auto const system_location =
                FVector(system.world_pos_x, system.world_pos_y, 0) *
                    ui_defaults().map_scale_;
            Amap_system * system_pawn = GetWorld()->SpawnActor<Amap_system>(
                system_class_,
                system_location,
                FRotator(),
                FActorSpawnParameters());
            configure_map_star(system_pawn, system);
            // TODO: Set the system ID in *system_pawn. (Hex ID too?)
        }
    }

    // TODO: Move controller pawn to the middle of the map (or the player's
    // home hex).

    if (auto * pc = player_controller()) {
        // TODO? pc->SetSpawnLocation
    }

    cast(GameState)->play_state_ = play_state::playing;
    cast(GameState)->play_state_changed();
}
