#include "Amap_hex.h"
#include "constants.hpp"

#include <Components/SceneComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Engine/CollisionProfile.h>


Amap_hex::Amap_hex()
{
    PrimaryActorTick.bCanEverTick = true;

    root_ = CreateDefaultSubobject<USceneComponent>(TEXT("root"));
    mesh_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"));
    hover_indicator_ =
        CreateDefaultSubobject<UStaticMeshComponent>(TEXT("hover_indicator"));
    selection_indicator_ = CreateDefaultSubobject<UStaticMeshComponent>(
        TEXT("selection_indicator"));

    RootComponent = root_;

    mesh_->SetupAttachment(root_);
    mesh_->SetVisibility(false);
    hover_indicator_->SetupAttachment(root_);
    hover_indicator_->SetHiddenInGame(true);
    selection_indicator_->SetupAttachment(root_);
    selection_indicator_->SetHiddenInGame(true);

    // collisions
    mesh_->SetCollisionProfileName(
        UCollisionProfile::CustomCollisionProfileName);
    mesh_->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    mesh_->SetCollisionResponseToChannel(
        ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Block);
    mesh_->SetCollisionResponseToChannel(
        hex_channel, ECollisionResponse::ECR_Block);
    hover_indicator_->SetCollisionProfileName(
        UCollisionProfile::NoCollision_ProfileName);
    selection_indicator_->SetCollisionProfileName(
        UCollisionProfile::NoCollision_ProfileName);

    // mobility
    root_->SetMobility(EComponentMobility::Static);
    mesh_->SetMobility(EComponentMobility::Static);
    hover_indicator_->SetMobility(EComponentMobility::Static);
    selection_indicator_->SetMobility(EComponentMobility::Static);
}

void Amap_hex::BeginPlay() { Super::BeginPlay(); }

void Amap_hex::Tick(float delta) { Super::Tick(delta); }

void Amap_hex::select(bool b)
{
    Amap_pawn_base::select(b);
    selection_indicator_->SetHiddenInGame(!b);
}

void Amap_hex::hover(bool b)
{
    Amap_pawn_base::hover(b);
    hover_indicator_->SetHiddenInGame(!b);
}
