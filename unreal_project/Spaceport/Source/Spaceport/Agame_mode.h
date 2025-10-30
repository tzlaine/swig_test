#pragma once

#include "game_data_t.hpp"

#include <CoreMinimal.h>
#include <GameFramework/GameModeBase.h>
#include "Agame_mode.generated.h"


class Agame_state;

UENUM(BlueprintType)
enum class play_state : uint8 {
    setup,            // SP, or game host in MP
    waiting_for_game, // non-hosts in MP
    playing,
    paused,
    ended
};

UCLASS()
class Agame_mode : public AGameModeBase
{
    GENERATED_BODY()

public:
    Agame_mode(FObjectInitializer const & init);

    virtual void BeginPlay() override;

private:
    void start_sp_game();
    void start_mp_game();

    game_data_t game_data_;
};
