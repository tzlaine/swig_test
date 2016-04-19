// Fill out your copyright notice in the Description page of Project Settings.

#include "Spaceport.h"
#include "hex_map.h"
#include "hex.h"
#include "utility.hpp"


Ahex_map::Ahex_map () :
    width(5),
    height(5)
{
    PrimaryActorTick.bCanEverTick = false;
}

void Ahex_map::BeginPlay ()
{
    Super::BeginPlay();
    call_real_soon(spawn_timer, this, &Ahex_map::spawn_hexes);
}

void Ahex_map::spawn_hexes()
{
    UWorld* const world = GetWorld();
    if (!hex || !world) {
        call_real_soon(spawn_timer, this, &Ahex_map::spawn_hexes);
        return;
    }

    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            Ahex* new_hex = spawn_hex(x, y, world);
            // TODO: Assign values to the hex.
        }
    }
}

Ahex* Ahex_map::spawn_hex (int x, int y, UWorld* const world)
{
    FVector location;
    location.X = x * 100.0f;
    location.Y = y * 100.0f;
    location.Z = 0.0f;

    FRotator rotation;

    FActorSpawnParameters spawn_params;
    spawn_params.Owner = this;
    spawn_params.Instigator = Instigator;

    return world->SpawnActor<Ahex>(hex, location, rotation, spawn_params);
}
