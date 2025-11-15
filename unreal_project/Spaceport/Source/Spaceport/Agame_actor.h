#pragma once

#include "constants.hpp"

#include <CoreMinimal.h>
#include <GameFramework/GameStateBase.h>
#include "Agame_actor.generated.h"


UCLASS()
class Agame_actor : public AActor
{
    GENERATED_BODY()

public:
    Agame_actor();

    int nation_id() const { return nation_id_; }

    void nation_id(int id) { nation_id_ = id; }

private:
    int nation_id_ = nation_none;
};
