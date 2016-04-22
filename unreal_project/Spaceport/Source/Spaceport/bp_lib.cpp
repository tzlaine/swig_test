// Fill out your copyright notice in the Description page of Project Settings.

#include "Spaceport.h"
#include "bp_lib.h"
#include "Kismet/KismetMathLibrary.h"


namespace {
	FVector const bad_intersection = {0, 0, -1000.0};
	FPlane const xy_plane(FVector::ZeroVector, FVector(0, 0, 1.0f));
}

FVector Ubp_lib::prev_map_drag_intersection_ = bad_intersection;
bool Ubp_lib::map_drag_viable_ = false;

void Ubp_lib::start_map_drag (APlayerController* pc)
{
	FVector origin, direction;
	if (!pc->DeprojectMousePositionToWorld(origin, direction)) {
		map_drag_viable_ = false;
		return;
	}
	FVector const terminus = origin + direction * 50000;

	float t;
	map_drag_viable_ =
		UKismetMathLibrary::LinePlaneIntersection(origin, terminus, xy_plane, t, prev_map_drag_intersection_);
}

FVector Ubp_lib::map_drag_offset(APlayerController* pc)
{
	FVector retval = { 0, 0, 0 };

	if (!map_drag_viable_)
		return retval;

	FVector origin, direction;
	if (!pc->DeprojectMousePositionToWorld(origin, direction)) {
		map_drag_viable_ = false;
		return retval;
	}
	FVector const terminus = origin + direction * 50000;
	
	float t;
	FVector intersection;
	if (UKismetMathLibrary::LinePlaneIntersection(origin, terminus, xy_plane, t, intersection)) {
		retval = prev_map_drag_intersection_ - intersection;
	} else {
		prev_map_drag_intersection_ = bad_intersection;
		map_drag_viable_ = false;
	}

	return retval;
}
