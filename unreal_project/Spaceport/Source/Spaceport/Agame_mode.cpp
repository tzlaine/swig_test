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
    PrimaryActorTick.bCanEverTick = true;
    UE_LOG(LogTemp, Log, TEXT("EXIT Agame_mode CTOR"));
}

void Agame_mode::BeginPlay()
{
    Super::BeginPlay();
    // TODO UE_LOG(LogTemp, Log, TEXT("ENTER Agame_mode::BeginPlay()"));
    // TODO if (Ugame_instance::get()->game_kind() == game_kind::sp)
    // TODO     ready_for_sp_game();
    // TODO else
    // TODO     ready_for_mp_game();
    // TODO UE_LOG(LogTemp, Log, TEXT("EXIT Agame_mode::BeginPlay()"));
}

void Agame_mode::EndPlay(EEndPlayReason::Type reason) // TODO
{
    // Explicitly NOT calling Super::EndPlay.
    AGameModeBase::EndPlay(reason);
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
