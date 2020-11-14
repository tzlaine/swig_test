// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "bp_lib.generated.h"

/**
 * 
 */
UCLASS()
class SPACEPORT_API Ubp_lib : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category = "cursor_hit_test")
    static FVector mouse_to_xy_plane (APlayerController* pc); // returns z != 0 on failure

    UFUNCTION(BlueprintCallable, Category = "zoom_pan")
    static FVector next_lookat_for_zoom_pan (APlayerController* pc, FVector next_lookat_point, float zoom_axis, float zoom_pan_factor);

    UFUNCTION(BlueprintCallable, Category = "zoom_pan")
    static void zoom_pan_timeout ();

    UFUNCTION(BlueprintCallable, Category = "map_drag")
    static void start_map_drag (APlayerController* pc);

    UFUNCTION(BlueprintPure, Category = "map_drag")
    static FVector map_drag_offset (APlayerController* pc);

    UFUNCTION(BlueprintPure, Category = "map_drag")
    static bool continuable_game_available ();

private:
    static FVector original_map_drag_intersection_;
    static bool map_drag_viable_;
    static FVector zoom_pan_lookat_;
};
