#include "Amap_fleet.h"
#include "Agame_mode.h"
#include "constants.hpp"

#include <Components/StaticMeshComponent.h>
#include <GameFramework/FloatingPawnMovement.h>
#include <Engine/CollisionProfile.h>
#include <Kismet/KismetMathLibrary.h>


Amap_fleet::Amap_fleet()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;

    root_ = CreateDefaultSubobject<USceneComponent>(TEXT("root"));
    hit_mesh_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("hit_mesh"));
    mesh_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"));
    hover_indicator_ =
        CreateDefaultSubobject<UStaticMeshComponent>(TEXT("hover_indicator"));
    selection_indicator_ = CreateDefaultSubobject<UStaticMeshComponent>(
        TEXT("selection_indicator"));
    movement_ = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("movement"));

    RootComponent = root_;

    hit_mesh_->SetupAttachment(root_);
    hit_mesh_->SetHiddenInGame(true);
    mesh_->SetupAttachment(root_);
    hover_indicator_->SetupAttachment(root_);
    hover_indicator_->SetHiddenInGame(true);
    selection_indicator_->SetupAttachment(root_);
    selection_indicator_->SetHiddenInGame(true);

    // collisions
    hit_mesh_->SetCollisionProfileName(
        UCollisionProfile::CustomCollisionProfileName);
    hit_mesh_->SetCollisionResponseToAllChannels(
        ECollisionResponse::ECR_Ignore);
    hit_mesh_->SetCollisionResponseToChannel(
        ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Block);
    hit_mesh_->SetCollisionResponseToChannel(
        fleet_channel, ECollisionResponse::ECR_Block);
    mesh_->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
    hover_indicator_->SetCollisionProfileName(
        UCollisionProfile::NoCollision_ProfileName);
    selection_indicator_->SetCollisionProfileName(
        UCollisionProfile::NoCollision_ProfileName);

    // shadows
    hit_mesh_->SetCastShadow(false);
    hit_mesh_->bReceiveMobileCSMShadows = false;
    mesh_->SetCastShadow(false);
    mesh_->bReceiveMobileCSMShadows = false;
    hover_indicator_->SetCastShadow(false);
    hover_indicator_->bReceiveMobileCSMShadows = false;
    selection_indicator_->SetCastShadow(false);
    selection_indicator_->bReceiveMobileCSMShadows = false;

    // ticks
    root_->SetComponentTickEnabled(false);
    hit_mesh_->SetComponentTickEnabled(false);
    mesh_->SetComponentTickEnabled(false);
    hover_indicator_->SetComponentTickEnabled(false);
    selection_indicator_->SetComponentTickEnabled(false);
    movement_->SetComponentTickEnabled(false);
}

void Amap_fleet::BeginPlay() { Super::BeginPlay(); }

void Amap_fleet::Tick(float delta)
{
    Super::Tick(delta);

    if (move_to_system_ != system_none)
        execute_map_move(delta);
}

void Amap_fleet::select(bool b)
{
    Amap_pawn_base::select(b);
    selection_indicator_->SetHiddenInGame(!b);
}

void Amap_fleet::hover(bool b)
{
    Amap_pawn_base::hover(b);
    hover_indicator_->SetHiddenInGame(!b);
}

void Amap_fleet::move_to_system(int system_id, FVector map_location)
{
    // TODO: upate the underlying fleet object in the model.
    move_to_system_ = system_id;
    move_to_map_location_ = map_location;
}

void Amap_fleet::execute_map_move(float delta)
{
    auto move_dir = move_to_map_location_ - GetActorLocation();
    if (move_dir.SquaredLength() < map_move_eps_ * map_move_eps_) {
        move_to_system_ = system_none;
        return;
    }
    move_dir.Normalize(1);

    // rotate
    FRotator move_rot = UKismetMathLibrary::MakeRotFromX(move_dir);
    move_rot.Pitch = move_rot.Roll = 0;
    FRotator rot = FMath::RInterpTo(
        GetActorRotation(), move_rot, delta, map_rotate_speed_);
    if (map_rotate_eps_ <
        std::abs(rot.Roll) + std::abs(rot.Pitch) + std::abs(rot.Yaw)) {
        SetActorRotation(rot);
        return;
    }

    // TODO: Need a game speed factor here.  We need to take the fleet speed
    // factor, convert it into units/game-day, then convert it further using
    // game-days/walltime-secs.

    // TODO: Wait to be ready to actually move; the length of time required
    // change based on tech level, and perhaps war preparedness.

    float const speed = 0.1f; // TODO: Get this from the fleet itself.

    AddMovementInput(move_dir, speed);
}
