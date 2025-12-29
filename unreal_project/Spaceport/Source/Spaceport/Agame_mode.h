#pragma once

#include "model.hpp"
#include "user_notification.h"

#include <vector>

#include <CoreMinimal.h>
#include <GameFramework/GameModeBase.h>
#include "Agame_mode.generated.h"


struct Ffile_change;
class Agame_state;
class Aplaying_hud;
class Amap_fleet;
class Amap_system;
class Amap_hex;
class Urepl_graph;

UCLASS()
class Agame_mode : public AGameModeBase
{
    GENERATED_BODY()

public:
    Agame_mode(FObjectInitializer const & init);

    boost::shared_ptr<game_state_t const> game_state() const
    {
        return model_->game_state();
    }

    void PostLogin(APlayerController * player) override;
    void BeginPlay() override;
    void Tick(float secs) override;
    void EndPlay(EEndPlayReason::Type reason) override;

    bool waiting_for_client_to_receive_initial_game_state(int nation_id) const;

    void ready_for_game();
    void load_and_start_newest_game();
    void load_and_start_game(FString const & filename);
    // Load game iff params.IsEmpty(); generate a new galaxy otherwise.
    void load_or_generate(TArray<uint8> const & params);
    void client_received_initial_game_state(int nation_id);

    void quit_to_menu(
        Fuser_notification const & notification = Fuser_notification());

    void publish_save_files();
    void save_game(FString const & filename);
    void toggle_pause();
    void play_speed(int speed);

    void notify_repl_graph_constructed(Urepl_graph * rg) { repl_graph_ = rg; }
    void notify_repl_graph_destroyed(Urepl_graph * rg)
    {
        if (rg == repl_graph_)
            repl_graph_ = nullptr;
    }

private:
    void saves_dir_changed(std::vector<Ffile_change> changes);
    void ready_for_sp_game();
    void ready_for_mp_game();
    void signal_start_of_play();
    void all_clients_received_initial_game_state();
    void tear_down_game();

    int players_ = 0;

    float seconds_since_last_day_tick_ = 0.0;
    std::shared_ptr<model> model_;

    Urepl_graph * repl_graph_;

    // game setup
    game_start_params_t game_params_;
    std::vector<int> client_game_state_nation_ids_;
    std::vector<Amap_hex *> hex_pawns_;
    std::vector<Amap_system *> system_pawns_;

    // generating the galaxy
    std::jthread generation_thread_;
    std::unique_ptr<concurrent_queue<int>> percent_complete_;
    std::atomic_bool generation_complete_ = false;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Actor classes",
        meta = (AllowPrivateAccess = "true"))
    TSubclassOf<Amap_fleet> fleet_pawn_class_;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Actor classes",
        meta = (AllowPrivateAccess = "true"))
    TSubclassOf<Amap_system> system_class_;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Actor classes",
        meta = (AllowPrivateAccess = "true"))
    TSubclassOf<Amap_hex> hex_class_;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Actor classes",
        meta = (AllowPrivateAccess = "true"))
    TSubclassOf<AActor> spiral_galaxy_arms_class_;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Actor classes",
        meta = (AllowPrivateAccess = "true"))
    TSubclassOf<AActor> galactic_core_glow_class_;
};
