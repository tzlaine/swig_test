// Fill out your copyright notice in the Description page of Project Settings.

#include "Spaceport.h"
#include "hex_map.h"


Ahex_map::Ahex_map () :
    width(5),
    height(5)
{
    PrimaryActorTick.bCanEverTick = false;
}

void Ahex_map::BeginPlay ()
{
    Super::BeginPlay();
}

void Ahex_map::Tick (float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void Ahex_map::SpawnHex (int x, int y)
{
    if (!hex)
        return;

    UWorld* const world = GetWorld();
    if (!world)
        return;

    FVector location;
    location.x = 0.0f;
    location.y = 0.0f;
    location.z = 0.0f;

    FRotator rotation;

    FActorSpawnParameters spawn_params;
    spawn_params.Owner = this;
    spawn_params.Instigator = Instigator;

    Ahex* = new_hex = world->SpawnActor<Ahex>(hex, location, rotation, spawn_params);
}
