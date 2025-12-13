#include "Agame_mode.h"
#include "Agame_state.h"
#include "Amap_fleet.h"
#include "Amap_hex.h"
#include "Amap_system.h"
#include "Aplaying_hud.h"
#include "Aplayer_controller.h"
#include "Aplayer_state.h"
#include "game_instance.h"
#include "map_util.hpp"
#include "materials.h"
#include "rng.hpp"
#include "space_creator_actor_config.hpp"
#include "ui_defaults.h"
#include "utility.hpp"

#include <filesystem>

#include <Materials/MaterialInstanceDynamic.h>
#include <Materials/MaterialInterface.h>


namespace {
    Agame_state * cast(AGameStateBase * base)
    {
        return Cast<Agame_state>(base);
    }

    void set_play_state(AGameStateBase * gs_, play_state s)
    {
        auto * gs = cast(gs_);
        gs->prev_play_state_ = gs->play_state_;
        gs->play_state_ = s;
        gs->play_state_changed();
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

    set_play_state(GameState, play_state::generating);
    percent_complete_ = std::make_unique<concurrent_queue<int>>();

    auto params = from_tarray<game_start_params_t>(params_);
    player_id_to_nation_id_ = params.player_id_to_nation_id;
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
    if (cast(GameState)->play_state_ == play_state::playing) {
        // TODO: Give a notification of duration of pause in MP.
        set_play_state(GameState, play_state::paused);
    } else if (cast(GameState)->play_state_ == play_state::paused) {
        set_play_state(GameState, play_state::playing);
    } else {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("Agame_mode::toggle_pause() Called when the game is not in "
                 "session."));
    }
}

void Agame_mode::play_speed(int speed)
{
    if (cast(GameState)->play_state_ != play_state::playing &&
        cast(GameState)->play_state_ != play_state::paused) {
        return;
    }
    // TODO: Give a notification of speed change a bit before changing it in
    // MP.
    speed = std::clamp(speed, 1, 5);
    model_.set_speed(speed);
    cast(GameState)->play_speed_ = speed;
    cast(GameState)->play_speed_changed();
}

void Agame_mode::ready_for_sp_game()
{
    std::filesystem::path load_path = Ugame_instance::get()->game_to_load();
    if (load_path.empty()) {
        set_play_state(GameState, play_state::setup);
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

    for (auto const & hex : model_.hexes()) {
        if (hex.province_id == prov_galactic_bulge ||
            hex.province_id == prov_off_map) {
            continue;
        }

        if (hex.province_id == prov_galactic_center) {
            // This assumes we're using the 'plane' mesh that comes with UE,
            // which is 100x100.
            double const plane_width = 100.0;

            auto * arms = GetWorld()->SpawnActor<AActor>(
                spiral_galaxy_arms_class_,
                FTransform(
                    FRotator(0, random_double(0, 360), 0),
                    map_hex_position(hex.coord, gs.map_height) +
                        FVector(0, 0, 20),
                    FVector(1)),
                FActorSpawnParameters());
            arms->SetActorScale3D(FVector(
                gs.map_height * hex_height / plane_width *
                ui_defaults().map_scale_));

            auto * glow = GetWorld()->SpawnActor<AActor>(
                galactic_core_glow_class_,
                FTransform(
                    FRotator(),
                    map_hex_position(hex.coord, gs.map_height) +
                        FVector(0, 0, 10),
                    FVector(1)),
                FActorSpawnParameters());
            glow->SetActorScale3D(FVector(
                12 * hex_height / plane_width * ui_defaults().map_scale_));

            continue;
        }

        auto const hex_location = map_hex_position(hex.coord, gs.map_height);
        Amap_hex * hex_pawn = GetWorld()->SpawnActor<Amap_hex>(
            hex_class_, hex_location, FRotator(), FActorSpawnParameters());
        hex_pawn->hex_id(to_index(hex.coord, gs.map_width));

        for (int i = hex.first_system, last = hex.last_system; i < last; ++i) {
            auto const & system = gs.systems[i];
            auto const system_location =
                FVector(system.world_pos_x, system.world_pos_y, 0) *
                    ui_defaults().map_scale_;
            Amap_system * system_pawn = GetWorld()->SpawnActor<Amap_system>(
                system_class_,
                system_location,
                FRotator(0, random_double(0, 360), 0),
                FActorSpawnParameters());
            configure_map_star(system_pawn, system);
            UMaterialInstanceDynamic * selected_mid =
                UMaterialInstanceDynamic::Create(
                    materials().system_selected_, system_pawn);
            selected_mid->SetVectorParameterValue(
                TEXT("color"), ui_defaults().system_selected_color_);
            UMaterialInstanceDynamic * hovered_mid =
                UMaterialInstanceDynamic::Create(
                    materials().system_selected_, system_pawn);
            hovered_mid->SetVectorParameterValue(
                TEXT("color"), ui_defaults().system_hovered_color_);
            system_pawn->selection_materials(selected_mid, hovered_mid);
            system_pawn->system_id(i);
        }
    }

    check(GetWorld());
    for (auto it = GetWorld()->GetPlayerControllerIterator(); it; ++it) {
        auto * pc = Cast<Aplayer_controller>(it->Get());
        check(pc);

        auto * ps = Cast<Aplayer_state>(pc->PlayerState);
        check(ps);
        int const nation_id = player_id_to_nation_id_[ps->player_id()];

        // TODO: Send full state.
        game_state_t game_state = {0};
        game_state.map_width = gs.map_width;
        game_state.map_height = gs.map_height;
        game_state.nations.resize(nation_id + 1);
        nation_t & nation = game_state.nations.back();
        nation.id = nation_id;
        auto const hc = hex_coord_t{gs.map_width / 2, gs.map_height / 2};
        nation.hexes_seen.push_back(to_index(hc, gs.map_width));
        TArray<uint8> state = to_tarray(game_state);
        pc->client_recv_initial_game_state(nation_id, state);
    }

    set_play_state(GameState, play_state::paused);
}
