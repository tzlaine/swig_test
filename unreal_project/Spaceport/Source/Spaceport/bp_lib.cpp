// Fill out your copyright notice in the Description page of Project Settings.

#include "bp_lib.h"
#include "Spaceport.h"
#include "Kismet/KismetMathLibrary.h"


namespace {
    FVector const bad_xy_point = {0, 0, -1000.0};
    FPlane const xy_plane(FVector::ZeroVector, FVector(0, 0, 1.0f));
    float const epsilon = 1.0e-3;

    bool near_zero(float x)
    {
        return fabs(x - 0.0f) < epsilon;
    }
}

FVector Ubp_lib::original_map_drag_intersection_ = bad_xy_point;
bool Ubp_lib::map_drag_viable_ = false;
FVector Ubp_lib::zoom_pan_lookat_ = bad_xy_point;

FVector Ubp_lib::mouse_to_xy_plane (APlayerController* pc) // TODO: Add bounds outside of which hits do not land.
{
    FVector retval = bad_xy_point;

    FVector origin, direction;
    if (!pc->DeprojectMousePositionToWorld(origin, direction))
        return retval;

    FVector const terminus = origin + direction * 50000;

    float t;
    if (!UKismetMathLibrary::LinePlaneIntersection(origin, terminus, xy_plane, t, retval)) {
        retval = bad_xy_point;
    }

    return retval;
}

FVector Ubp_lib::next_lookat_for_zoom_pan (APlayerController* pc, FVector next_lookat_point, float zoom_axis, float zoom_pan_factor)
{
    FVector retval = bad_xy_point;

    if (zoom_axis <= 0) {
        if (zoom_axis < 0)
            zoom_pan_lookat_ = bad_xy_point;
        return retval;
    }

    if (zoom_pan_lookat_ == bad_xy_point) {
        zoom_pan_lookat_ = mouse_to_xy_plane(pc);
        if (!near_zero(zoom_pan_lookat_.Z)) {
            zoom_pan_lookat_ = bad_xy_point;
            return retval;
        }
    }

    float const remainder_factor = 1.0f - zoom_pan_factor;
    float const alpha = FMath::Pow(remainder_factor, zoom_axis);

    retval = alpha * next_lookat_point + (1 - alpha) * zoom_pan_lookat_;

    return retval;
}

void Ubp_lib::zoom_pan_timeout ()
{
    //GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::White, TEXT("zoom_pan_lookat_ reset!"));
    zoom_pan_lookat_ = bad_xy_point;
}

void Ubp_lib::start_map_drag (APlayerController* pc)
{
    original_map_drag_intersection_ = mouse_to_xy_plane(pc);
    map_drag_viable_ = near_zero(original_map_drag_intersection_.Z);
}

FVector Ubp_lib::map_drag_offset(APlayerController* pc)
{
    FVector retval = { 0, 0, 0 };

    if (!map_drag_viable_)
        return retval;

    FVector intersection = mouse_to_xy_plane(pc);
    if (near_zero(intersection.Z)) {
        retval = original_map_drag_intersection_ - intersection;
    } else {
        original_map_drag_intersection_ = bad_xy_point;
        map_drag_viable_ = false;
    }

    return retval;
}
