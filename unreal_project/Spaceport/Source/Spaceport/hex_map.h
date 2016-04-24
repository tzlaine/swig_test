// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "hex_map.generated.h"

UCLASS()
class SPACEPORT_API Ahex_map : public AActor
{
    GENERATED_BODY()

public:
    Ahex_map ();

    virtual void BeginPlay () override;

protected:
    void spawn_hexes ();
    class Ahex* spawn_hex (int x, int y, int height, UWorld* const world);

    FTimerHandle spawn_timer;

    UPROPERTY(EditAnywhere, Category = "Hex")
    TSubclassOf<class Ahex> hex;
};
