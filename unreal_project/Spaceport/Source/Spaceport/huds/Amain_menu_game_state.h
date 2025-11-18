#pragma once

#include "dir_watcher.h"
#include "Agame_state_base.h"

#include <CoreMinimal.h>
#include <GameFramework/GameStateBase.h>
#include "Amain_menu_game_state.generated.h"


UCLASS()
class Amain_menu_game_state : public Agame_state_base
{
    GENERATED_BODY()

public:
    Amain_menu_game_state();
};
