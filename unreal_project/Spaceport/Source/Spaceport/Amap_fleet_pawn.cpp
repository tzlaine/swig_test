#include "Amap_fleet_pawn.h"


Amap_fleet_pawn::Amap_fleet_pawn() { PrimaryActorTick.bCanEverTick = true; }

void Amap_fleet_pawn::BeginPlay() { Super::BeginPlay(); }

void Amap_fleet_pawn::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void Amap_fleet_pawn::SetupPlayerInputComponent(
    UInputComponent * PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}
