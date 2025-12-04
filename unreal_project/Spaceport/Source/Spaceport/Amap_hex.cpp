#include "Amap_hex.h"

#include <Components/CapsuleComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Engine/CollisionProfile.h>


Amap_hex::Amap_hex()
{
    PrimaryActorTick.bCanEverTick = true;

    capsule_ = CreateDefaultSubobject<UCapsuleComponent>(TEXT("capsule"));
    mesh_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"));
    selection_indicator_ = CreateDefaultSubobject<UStaticMeshComponent>(
        TEXT("selection_indicator"));

    RootComponent = capsule_;

    capsule_->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
    mesh_->SetupAttachment(RootComponent);
    selection_indicator_->SetupAttachment(RootComponent);
    selection_indicator_->SetHiddenInGame(true);
    selection_indicator_->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void Amap_hex::BeginPlay() { Super::BeginPlay(); }

void Amap_hex::Tick(float delta) { Super::Tick(delta); }

void Amap_hex::select(bool b) { selection_indicator_->SetHiddenInGame(!b); }
