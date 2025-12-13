#pragma once

#include "Agame_mode_base.h"
#include "Agame_mode_fwd.h"

#include "model.hpp"

#include <CoreMinimal.h>
#include <GameFramework/GameModeBase.h>
#include "Agame_mode.generated.h"


class Agame_state;
class Aplaying_hud;
class Amap_fleet;
class Amap_system;
class Amap_hex;

UCLASS()
class Agame_mode : public Agame_mode_base
{
    GENERATED_BODY()

public:
    Agame_mode(FObjectInitializer const & init);

    boost::shared_ptr<game_state_t const> game_state() const
    {
        return model_.game_state();
    }

    void BeginPlay() override;
    void EndPlay(EEndPlayReason::Type reason) override;

    void save_game(FString const & filename);
    void toggle_pause();
    void play_speed(int speed);

private:
    float seconds_since_last_day_tick_ = 0.0;
};
