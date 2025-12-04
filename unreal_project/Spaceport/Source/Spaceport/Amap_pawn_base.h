#pragma once

#include "constants.hpp"
#include "map_util.hpp"

#include <CoreMinimal.h>
#include <GameFramework/Pawn.h>
#include "Amap_pawn_base.generated.h"


struct game_state_t;
struct point_2d;

enum struct map_pawn_kind { unknown, hex, system, fleet };

UCLASS()
class Amap_pawn_base : public APawn
{
    GENERATED_BODY()

public:
    Amap_pawn_base();

    int nation_id() const { return nation_id_; }

    void nation_id(int id) { nation_id_ = id; }
    virtual void select(bool b);
    virtual void hover(bool b);
    virtual map_pawn_kind kind() const { return map_pawn_kind::unknown; }

    static void deselect_current();
    static void dehover_current();

private:
    int nation_id_ = nation_none;

    static Amap_pawn_base * curr_selected_;
    static Amap_pawn_base * curr_hovered_;
};

inline Amap_pawn_base * Amap_pawn_base::curr_selected_ = nullptr;
inline Amap_pawn_base * Amap_pawn_base::curr_hovered_ = nullptr;

inline int owner(Amap_pawn_base const * p) { return p->nation_id(); }
inline point_2d position(Amap_pawn_base const * p)
{
    auto const pos = p->GetActorLocation();
    return {pos.X, pos.Y};
}
double detection_dist_sq(
    game_state_t const & gs,
    Amap_pawn_base const * detector,
    Amap_pawn_base const * other_pawn);
