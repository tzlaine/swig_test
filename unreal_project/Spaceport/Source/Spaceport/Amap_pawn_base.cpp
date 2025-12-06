#include "Amap_pawn_base.h"

#include "game_data.hpp"

// TODO: These includes and functions should go in their own TU.
#include "Amap_fleet.h"
#include "Amap_system.h"

double fleet_detecting_fleet_detection_dist_sq(
    game_state_t const & gs,
    Amap_fleet const * detector,
    Amap_fleet const * other_fleet)
{
    // TODO: Detection logic goes here.  NOTE: Must match logic in
    // model.hpp!
    double dist = 1.0;
    return dist * dist;
}

double system_detecting_fleet_detection_dist_sq(
    game_state_t const & gs,
    Amap_system const * detector,
    Amap_fleet const * other_fleet)
{
    // TODO: Detection logic goes here.  NOTE: Must match logic in
    // model.hpp!
    double dist = 1.0;
    return dist * dist;
}

namespace {
    double detection_dist_sq_impl(
        game_state_t const & gs,
        Amap_pawn_base const * detector,
        map_pawn_kind detector_kind,
        Amap_pawn_base const * other_pawn,
        map_pawn_kind other_pawn_kind)
    {
        if (detector_kind == map_pawn_kind::fleet &&
            other_pawn_kind == map_pawn_kind::fleet) {
            return fleet_detecting_fleet_detection_dist_sq(
                gs, Cast<Amap_fleet>(detector), Cast<Amap_fleet>(other_pawn));
        } else if (
            detector_kind == map_pawn_kind::system &&
            other_pawn_kind == map_pawn_kind::fleet) {
            return system_detecting_fleet_detection_dist_sq(
                gs, Cast<Amap_system>(detector), Cast<Amap_fleet>(other_pawn));
        } else {
            return 0.0;
        }
    }

    void curr_pawn_impl(bool b, Amap_pawn_base *& curr, Amap_pawn_base * self)
    {
        if (!b) {
            check(!curr || curr == self);
            curr = nullptr;
            return;
        }
        if (curr)
            curr->select(false);
        curr = self;
    }
}

Amap_pawn_base::Amap_pawn_base() {}

void Amap_pawn_base::select(bool b) {}

void Amap_pawn_base::hover(bool b) {}

double detection_dist_sq(
    game_state_t const & gs,
    Amap_pawn_base const * detector,
    Amap_pawn_base const * other_pawn)
{
    return detection_dist_sq_impl(
        gs, detector, detector->kind(), other_pawn, other_pawn->kind());
}
