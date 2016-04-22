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
	UFUNCTION(BlueprintCallable, Category = "map_drag")
	static void start_map_drag (APlayerController* pc);

	UFUNCTION(BlueprintPure, Category = "map_drag")
    static FVector map_drag_offset (APlayerController* pc);

private:
	static FVector prev_map_drag_intersection_;
	static bool map_drag_viable_;
};
