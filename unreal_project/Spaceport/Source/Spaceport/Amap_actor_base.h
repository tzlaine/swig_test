#pragma once

#include "constants.hpp"
#include "map_util.hpp"

#include <CoreMinimal.h>
#include <GameFramework/GameStateBase.h>
#include "Amap_actor_base.generated.h"


struct game_state_t;
struct point_2d;

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

inline int owner(Amap_actor_base const * p) { return p->nation_id(); }
inline point_2d position(Amap_actor_base const * p)
{
    auto const pos = p->GetActorLocation();
    return {pos.X, pos.Y};
}
double detection_dist_sq(
    game_state_t const & gs,
    Amap_actor_base const * detector,
    Amap_actor_base const * other_actor);
