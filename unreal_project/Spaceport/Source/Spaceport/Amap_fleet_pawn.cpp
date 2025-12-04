#include "Amap_fleet_pawn.h"

#include <Components/CapsuleComponent.h>
#include <Components/StaticMeshComponent.h>
#include <GameFramework/FloatingPawnMovement.h>
#include <Engine/CollisionProfile.h>


Amap_fleet_pawn::Amap_fleet_pawn()
{
    PrimaryActorTick.bCanEverTick = true;

    capsule_ = CreateDefaultSubobject<UCapsuleComponent>(TEXT("capsule"));
    mesh_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"));
    selection_indicator_ = CreateDefaultSubobject<UStaticMeshComponent>(
        TEXT("selection_indicator"));
    movement_ = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("movement"));

    RootComponent = capsule_;

    capsule_->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
    mesh_->SetupAttachment(RootComponent);
    selection_indicator_->SetupAttachment(RootComponent);
    selection_indicator_->SetHiddenInGame(true);
    selection_indicator_->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void Amap_fleet_pawn::BeginPlay() { Super::BeginPlay(); }

void Amap_fleet_pawn::Tick(float delta) { Super::Tick(delta); }

void Amap_fleet_pawn::select(bool b) { selection_indicator_->SetHiddenInGame(!b); }
