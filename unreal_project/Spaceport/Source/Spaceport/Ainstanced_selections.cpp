#include "Ainstanced_selections.h"


Ainstanced_selections::Ainstanced_selections()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = false;

    selected_systems_ = CreateDefaultSubobject<UInstancedStaticMeshComponent>(
        TEXT("selected_systems"));
    hovered_systems_ = CreateDefaultSubobject<UInstancedStaticMeshComponent>(
        TEXT("hovered_systems"));

    RootComponent->SetMobility(EComponentMobility::Static);

    selected_systems_->SetupAttachment(RootComponent);
    hovered_systems_->SetupAttachment(RootComponent);

    // collisions
    selected_systems_->SetCollisionProfileName(
        UCollisionProfile::NoCollision_ProfileName);
    hovered_systems_->SetCollisionProfileName(
        UCollisionProfile::NoCollision_ProfileName);

    // mobility
    selected_systems_->SetMobility(EComponentMobility::Static);
    hovered_systems_->SetMobility(EComponentMobility::Static);

    // shadows
    selected_systems_->SetCastShadow(false);
    hovered_systems_->bReceiveMobileCSMShadows = false;

    // ticks
    selected_systems_->SetComponentTickEnabled(false);
    hovered_systems_->SetComponentTickEnabled(false);
}

void Ainstanced_selections::BeginPlay() { Super::BeginPlay(); }

void select(Amap_fleet * f);
void deselect(Amap_system * f);
void hover(Amap_fleet * f);
void dehover(Amap_system * f);
void dehover_all();
