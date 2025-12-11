#include "Amap_system.h"
#include "constants.hpp"
#include "utility.hpp"

#include <Components/SphereComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Components/TextRenderComponent.h>
#include <Engine/CollisionProfile.h>
#include <Materials/MaterialInstanceDynamic.h>


Amap_system::Amap_system()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    root_ = CreateDefaultSubobject<USceneComponent>(TEXT("root"));
    sphere_ = CreateDefaultSubobject<USphereComponent>(TEXT("sphere"));
    mesh_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"));
    hover_indicator_ =
        CreateDefaultSubobject<UStaticMeshComponent>(TEXT("hover_indicator"));
    selection_indicator_ = CreateDefaultSubobject<UStaticMeshComponent>(
        TEXT("selection_indicator"));
    system_name_ =
        CreateDefaultSubobject<UTextRenderComponent>(TEXT("system_name"));

    RootComponent = root_;

    sphere_->SetupAttachment(RootComponent);
    sphere_->SetSphereRadius(1.0f);
    mesh_->SetupAttachment(root_);
    hover_indicator_->SetupAttachment(root_);
    hover_indicator_->SetHiddenInGame(true);
    selection_indicator_->SetupAttachment(root_);
    selection_indicator_->SetHiddenInGame(true);
    system_name_->SetupAttachment(root_);
    system_name_->SetHiddenInGame(true);

    // collisions
    sphere_->SetCollisionProfileName(
        UCollisionProfile::CustomCollisionProfileName);
    sphere_->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    sphere_->SetCollisionResponseToChannel(
        ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Block);
    sphere_->SetCollisionResponseToChannel(
        star_channel, ECollisionResponse::ECR_Block);
    mesh_->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
    hover_indicator_->SetCollisionProfileName(
        UCollisionProfile::NoCollision_ProfileName);
    selection_indicator_->SetCollisionProfileName(
        UCollisionProfile::NoCollision_ProfileName);
    system_name_->SetCollisionProfileName(
        UCollisionProfile::NoCollision_ProfileName);

    // mobility
    root_->SetMobility(EComponentMobility::Static);
    sphere_->SetMobility(EComponentMobility::Static);
    mesh_->SetMobility(EComponentMobility::Static);
    hover_indicator_->SetMobility(EComponentMobility::Static);
    selection_indicator_->SetMobility(EComponentMobility::Static);
    system_name_->SetMobility(EComponentMobility::Static);

    // shadows
    sphere_->SetCastShadow(false);
    sphere_->bReceiveMobileCSMShadows = false;
    mesh_->SetCastShadow(false);
    mesh_->bReceiveMobileCSMShadows = false;
    hover_indicator_->SetCastShadow(false);
    hover_indicator_->bReceiveMobileCSMShadows = false;
    selection_indicator_->SetCastShadow(false);
    selection_indicator_->bReceiveMobileCSMShadows = false;
    system_name_->SetCastShadow(false);
    system_name_->bReceiveMobileCSMShadows = false;

    // ticks
    root_->SetComponentTickEnabled(false);
    sphere_->SetComponentTickEnabled(false);
    mesh_->SetComponentTickEnabled(false);
    hover_indicator_->SetComponentTickEnabled(false);
    selection_indicator_->SetComponentTickEnabled(false);
    system_name_->SetComponentTickEnabled(false);
}

void Amap_system::BeginPlay() { Super::BeginPlay(); }

void Amap_system::Tick(float delta) { Super::Tick(delta); }

void Amap_system::select(bool b)
{
    selection_indicator_->SetHiddenInGame(!b);
}

void Amap_system::hover(bool b)
{
    hover_indicator_->SetHiddenInGame(!b);
}

void Amap_system::main_material(UMaterialInstanceDynamic * mid)
{
    main_mid_ = mid;
    mesh_->SetMaterial(0, main_mid_);
}

void Amap_system::selection_materials(
    UMaterialInstanceDynamic * selected, UMaterialInstanceDynamic * hovered)
{
    selected_mid_ = selected;
    hovered_mid_ = hovered;
    selection_indicator_->SetMaterial(0, selected_mid_);
    hover_indicator_->SetMaterial(0, hovered_mid_);
}
