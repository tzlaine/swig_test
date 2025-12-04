#include "Amap_system.h"
#include "constants.hpp"

#include <Components/SphereComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Engine/CollisionProfile.h>


Amap_system::Amap_system()
{
    PrimaryActorTick.bCanEverTick = true;

    sphere_ = CreateDefaultSubobject<USphereComponent>(TEXT("sphere"));
    mesh_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"));
    selection_indicator_ = CreateDefaultSubobject<UStaticMeshComponent>(
        TEXT("selection_indicator"));

    RootComponent = sphere_;

    sphere_->SetSphereRadius(1.0f);
    mesh_->SetupAttachment(RootComponent);
    selection_indicator_->SetupAttachment(RootComponent);
    selection_indicator_->SetHiddenInGame(true);

    // collisions
    sphere_->SetCollisionProfileName(
        UCollisionProfile::CustomCollisionProfileName);
    sphere_->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    sphere_->SetCollisionResponseToChannel(
        ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Block);
    sphere_->SetCollisionResponseToChannel(
        star_channel, ECollisionResponse::ECR_Block);
    mesh_->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
    selection_indicator_->SetCollisionProfileName(
        UCollisionProfile::NoCollision_ProfileName);

    // mobility
    sphere_->SetMobility(EComponentMobility::Static);
    mesh_->SetMobility(EComponentMobility::Static);
    selection_indicator_->SetMobility(EComponentMobility::Static);
}

void Amap_system::BeginPlay() { Super::BeginPlay(); }

void Amap_system::Tick(float delta) { Super::Tick(delta); }

void Amap_system::select(bool b)
{
    Amap_pawn_base::select(b);
    selection_indicator_->SetHiddenInGame(!b);
}

void Amap_system::hover(bool b)
{
    Amap_pawn_base::hover(b);
    selection_indicator_->SetHiddenInGame(!b);
    // TODO: separate hover indicator
}
