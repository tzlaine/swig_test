#include "Agame_mode.h"
#include "Agame_state.h"
#include "Amap_fleet.h"
#include "Amap_hex.h"
#include "Amap_system.h"
#include "Aplayer_controller.h"
#include "Aplayer_state.h"
#include "Urepl_graph.h"
#include "game_instance.h"
#include "lua.hpp"
#include "map_util.hpp"
#include "materials.h"
#include "rng.hpp"
#include "space_creator_actor_config.hpp"
#include "ui_defaults.h"
#include "user_notification.h"
#include "utility.hpp"

#include <filesystem>

#include <Materials/MaterialInstanceDynamic.h>
#include <Materials/MaterialInterface.h>


namespace {
    Agame_state * cast(AGameStateBase * base)
    {
        return Cast<Agame_state>(base);
    }

    void set_play_state(
        AGameStateBase * gs_,
        play_state s,
        Fuser_notification const & notification = Fuser_notification())
    {
        auto * gs = cast(gs_);
        gs->prev_play_state_ = gs->play_state_;
        gs->play_state_ = s;
        gs->deferred_notification_ = notification;
        gs->play_state_changed();
    }

    template<typename T>
    void destroy_actors_of_class(
        UWorld * w, TSubclassOf<T> subclass, TArray<AActor *> & actors)
    {
        UGameplayStatics::GetAllActorsOfClass(w, subclass, actors);
        for (auto * a : actors) {
            a->Destroy();
        }
    }
}

Agame_mode::Agame_mode(FObjectInitializer const & init) :
    AGameModeBase(init), model_(std::make_shared<model>())
{
    UE_LOG(LogTemp, Log, TEXT("ENTER Agame_mode CTOR"));
    PrimaryActorTick.bCanEverTick = true;
    UE_LOG(LogTemp, Log, TEXT("EXIT Agame_mode CTOR"));
}

void Agame_mode::PostLogin(APlayerController * player)
{
    Super::PostLogin(player);

    UE_LOG(LogTemp, Log, TEXT("Player %d logged in."), players_);

    if (!player)
        return;

    auto * ps = Cast<Aplayer_state>(player->PlayerState);
    if (!ps)
        return;

    ps->player_id(players_);
    ++players_;
}

void Agame_mode::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Log, TEXT("ENTER Agame_mode::BeginPlay()"));
    publish_save_files();
    Ugame_instance::get()->watch_save_game_dir(
        [this](auto changes) { saves_dir_changed(std::move(changes)); });
#if WITH_EDITOR
    reset_all_lua_states(); // Force the Lua scripts to be reloaded.
#endif
    UE_LOG(LogTemp, Log, TEXT("EXIT Agame_mode::BeginPlay()"));
}

void Agame_mode::Tick(float dt)
{
    Super::Tick(dt);

    if (cast(GameState)->play_state_ == play_state::generating) {
        int percent_update = 0;
        if (percent_complete_->try_pop(percent_update)) {
            player_controller()->client_galaxy_generation_update(
                percent_update);
        }

        if (generation_complete_) {
            model_->generate_after_galaxy(game_params_);
            signal_start_of_play();
        }
    }

    if (cast(GameState)->play_state_ != play_state::playing)
        return;

    // TODO: Update moving actors on the map; do the next day/month/year tick(s)
    // if necessary; send out model updates to clients.

    float const seconds_per_day =
        seconds_per_day_tick(cast(GameState)->play_speed_);
    ddays const days_this_tick(dt / seconds_per_day);
    day_progress_ += days_this_tick;
    if (ddays(1.0) < day_progress_) {
        auto * pc = player_controller();
        day_progress_ -= ddays(1.0);

        day_update_t day_update;
        auto const [new_month, new_year] = model_->day_tick(day_update);
        {
            TArray<uint8> day_state;
            detail::ostream_tarray_facade day_os(day_state);
            serialize_message(day_update, day_os);
            pc->client_recv_day_updates(day_state);
        }

        if (new_month) {
            month_update_t month_update;
            model_->month_tick(month_update);
            TArray<uint8> month_state;
            detail::ostream_tarray_facade month_os(month_state);
            serialize_message(month_update, month_os);
            pc->client_recv_month_updates(month_state);
        }

        if (new_year) {
            year_update_t year_update;
            model_->year_tick(year_update);
            TArray<uint8> year_state;
            detail::ostream_tarray_facade year_os(year_state);
            serialize_message(year_update, year_os);
            pc->client_recv_year_updates(year_state);
        }
    }
}

void Agame_mode::EndPlay(EEndPlayReason::Type reason)
{
    UE_LOG(LogTemp, Log, TEXT("ENTER Agame_mode::EndPlay()"));
    Super::EndPlay(reason);

    Ugame_instance::get()->unwatch_save_game_dir(); // TODO
    UE_LOG(LogTemp, Log, TEXT("EXIT Agame_mode::EndPlay()"));
}

bool Agame_mode::waiting_for_client_to_receive_initial_game_state(
    int nation_id) const
{
    return std::ranges::find(client_game_state_nation_ids_, nation_id) !=
           client_game_state_nation_ids_.end();
}

void Agame_mode::ready_for_game()
{
    UE_LOG(LogTemp, Log, TEXT("ENTER Agame_mode::ready_for_game()"));
    if (Ugame_instance::get()->game_kind() == game_kind::sp)
        ready_for_sp_game();
    else
        ready_for_mp_game();
    UE_LOG(LogTemp, Log, TEXT("EXIT Agame_mode::ready_for_game()"));
}

void Agame_mode::load_and_start_newest_game()
{
    TArray<FString> const saves = find_save_files(true);
    std::filesystem::path newest;

    if (!saves.IsEmpty()) {
        std::filesystem::path const dir = save_dir_path();
        newest = dir / to_path(saves[0]);
        std::filesystem::file_time_type newest_t = last_write_time(newest);
        for (auto const & save : saves) {
            std::filesystem::path p = dir / to_path(save);
            auto const t = last_write_time(p);
            if (newest_t < t) {
                newest = std::move(p);
                newest_t = t;
            }
        }
    }

    if (newest.empty()) {
        if (auto * pc = player_controller()) {
            pc->clients_notify_users(Fuser_notification(
                TEXT("load_game_failed"), TEXT("continue_failed_no_saves")));
        }
    } else {
        Ugame_instance::get()->game_to_load(newest);
        ready_for_game();
    }
}

void Agame_mode::load_and_start_game(FString const & filename)
{
    set_play_state(GameState, play_state::loading);
    tear_down_game();
    auto f = to_path(filename);
    f += TEXT(".sav");
    Ugame_instance::get()->game_to_load(save_dir_path() / f);
    ready_for_game();
}

void Agame_mode::load_or_generate(TArray<uint8> const & params)
{
    if (params.IsEmpty()) {
        signal_start_of_play();
        return;
    }

    set_play_state(GameState, play_state::generating);
    percent_complete_ = std::make_unique<concurrent_queue<int>>();

    game_params_ = from_tarray<game_start_params_t>(params);
    generation_thread_ =
        std::jthread([&, params = std::move(game_params_), this] {
            model_->generate_galaxy(
                params, *percent_complete_, generation_complete_);
        });
}

void Agame_mode::client_received_initial_game_state(int nation_id)
{
    std::erase(client_game_state_nation_ids_, nation_id);
    if (client_game_state_nation_ids_.empty())
        all_clients_received_initial_game_state();
}

void Agame_mode::quit_to_menu(Fuser_notification const & notification)
{
    tear_down_game();
    set_play_state(GameState, play_state::start_menu, notification);
}

void Agame_mode::publish_save_files()
{
    TArray<FString> saves = find_save_files();

    // Filter out known non-game .sav files.
    auto const first = begin(saves);
    auto const last = end(saves);
    auto it = std::remove_if(first, last, [](auto const & e) {
        return e == TEXT("EnhancedInputUserSettings");
    });
    if (it != last)
        saves.SetNum(saves.Num() - (last - it));

    UE_LOG(
        LogTemp,
        Log,
        TEXT("Publish save files: %s"),
        *FString::Join(saves, TEXT(", ")));
    cast(GameState)->saves_ = std::move(saves);
    cast(GameState)->saves_changed();
}

void Agame_mode::save_game(FString const & filename)
{
    auto f = to_path(filename);
    f += TEXT(".sav");
    model_->save(save_dir_path() / f);
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
    model_->set_speed(speed);
    cast(GameState)->play_speed_ = speed;
    cast(GameState)->play_speed_changed();
}

void Agame_mode::saves_dir_changed(std::vector<Ffile_change> changes)
{
    for (auto const & c : changes) {
        UE_LOG(
            LogTemp,
            Log,
            TEXT("File change detected: %s, Change Type: %d"),
            *c.file,
            (int)c.kind);
    }

    publish_save_files();
    cast(GameState)->save_file_changes_.SetNum(changes.size());
    std::ranges::move(changes, begin(cast(GameState)->save_file_changes_));
    cast(GameState)->save_file_changes_changed();
}

void Agame_mode::ready_for_sp_game()
{
    std::filesystem::path load_path = Ugame_instance::get()->game_to_load();
    if (load_path.empty()) {
        set_play_state(GameState, play_state::setup);
    } else {
        try {
            model_->load(load_path);
            load_or_generate(TArray<uint8>{});
        } catch (failed_deserialization const & e) {
            FText message = FText::Format(
                loc_text(TEXT("load_game_failed_message")),
                FText::FromString(FString(e.what())));
            // TODO: This is flaky.  We should instead send a message to the
            // client, and then have it defer the quit_to_menu call until the
            // user hits 'Ok'.
            quit_to_menu(Fuser_notification(TEXT("load_game_failed"), message));
        }
    }
}

void Agame_mode::ready_for_mp_game()
{
    // TODO
}

void Agame_mode::signal_start_of_play()
{
    check(GetWorld());

    check(repl_graph_);
    repl_graph_->use_model(model_);

    for (auto it = GetWorld()->GetPlayerControllerIterator(); it; ++it) {
        auto * pc = Cast<Aplayer_controller>(it->Get());
        check(pc);

        auto * ps = Cast<Aplayer_state>(pc->PlayerState);
        check(ps);

        int const nation_id =
            game_params_.player_id_to_nation_id[ps->player_id()];
        pc->nation_id(nation_id);
        repl_graph_->team_insert(pc);
    }

    check(model_->game_state());
    auto const & gs = *model_->game_state();

    hex_pawns_.clear();
    system_pawns_.clear();
    hex_pawns_.reserve(model_->hexes().size());
    system_pawns_.reserve(model_->systems().size());

    for (auto const & hex : model_->hexes()) {
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
                        FVector(0, 0, 20 + map_actors_vertical_offset),
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
                        FVector(0, 0, 10 + map_actors_vertical_offset),
                    FVector(1)),
                FActorSpawnParameters());
            glow->SetActorScale3D(FVector(
                12 * hex_height / plane_width * ui_defaults().map_scale_));

            continue;
        }

        auto const hex_position = map_hex_position(hex.coord, gs.map_height) +
                                  FVector(0, 0, map_actors_vertical_offset);
        Amap_hex * hex_pawn = GetWorld()->SpawnActor<Amap_hex>(
            hex_class_, hex_position, FRotator(), FActorSpawnParameters());
        hex_pawn->id(to_index(hex.coord, gs.map_width));
        hex_pawns_.push_back(hex_pawn);

        for (int i = hex.first_system, last = hex.last_system; i < last; ++i) {
            auto const & system = gs.systems[i];
            auto const system_position =
                FVector(system.world_pos_x, system.world_pos_y, 0) *
                    ui_defaults().map_scale_ +
                FVector(0, 0, map_actors_vertical_offset);
            Amap_system * system_pawn = GetWorld()->SpawnActor<Amap_system>(
                system_class_,
                system_position,
                FRotator(),
                FActorSpawnParameters());
            system_pawn->id(i);
            system_pawn->generate_graphical_properties(system);
            system_pawns_.push_back(system_pawn);
        }
    }

    client_game_state_nation_ids_.clear();
    for (auto it = GetWorld()->GetPlayerControllerIterator(); it; ++it) {
        auto * pc = Cast<Aplayer_controller>(it->Get());
        check(pc);

        auto * ps = Cast<Aplayer_state>(pc->PlayerState);
        check(ps);
        int const nation_id =
            game_params_.player_id_to_nation_id[ps->player_id()];

        TArray<uint8> state;
        detail::ostream_tarray_facade os(state);
        model_->serialize_for_client(nation_id, os);
        pc->client_recv_initial_game_state(nation_id, state);
        client_game_state_nation_ids_.push_back(nation_id);
    }

    set_play_state(GameState, play_state::paused);
    cast(GameState)->play_speed_ = gs.play_speed;
}

void Agame_mode::all_clients_received_initial_game_state()
{
    check(repl_graph_);

    // Wait until the model has been pushed to the clients to do this, so that
    // here, just as in the middle of the game, the client has whatever
    // knowledge it is supposed to have about these hexes and systems before
    // seeing their pawns.
    for (auto * hex_pawn : hex_pawns_) {
        repl_graph_->reinsert_actor(hex_pawn);
        hex_pawn->OnRep_initial_properties();
    }
    for (auto * system_pawn : system_pawns_) {
        repl_graph_->reinsert_actor(system_pawn);
        system_pawn->OnRep_initial_properties();
    }

    hex_pawns_.clear();
    system_pawns_.clear();
}

void Agame_mode::tear_down_game()
{
    // despawn all our map actors
    TArray<AActor *> actors;
    destroy_actors_of_class(GetWorld(), fleet_pawn_class_, actors);
    destroy_actors_of_class(GetWorld(), system_class_, actors);
    destroy_actors_of_class(GetWorld(), hex_class_, actors);
    destroy_actors_of_class(GetWorld(), spiral_galaxy_arms_class_, actors);
    destroy_actors_of_class(GetWorld(), galactic_core_glow_class_, actors);

    model_ = std::make_unique<model>();
    generation_complete_ = false;
}
