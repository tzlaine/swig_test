#pragma once

#include "constants.hpp"

#include <CoreMinimal.h>
#include <GameFramework/GameStateBase.h>
#include "Amap_actor_base.generated.h"


UCLASS()
class Amap_actor_base : public AActor
{
    GENERATED_BODY()

public:
    Amap_actor_base();

    int nation_id() const { return nation_id_; }

    void nation_id(int id) { nation_id_ = id; }

private:
    int nation_id_ = nation_none;
};
