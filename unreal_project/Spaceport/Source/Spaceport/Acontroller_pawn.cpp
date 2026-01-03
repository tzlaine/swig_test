#include "Acontroller_pawn.h"

#include "constants.hpp"

#include <cmath>

#include <Camera/CameraComponent.h>
#include <Components/CapsuleComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <GameFramework/FloatingPawnMovement.h>


Acontroller_pawn::Acontroller_pawn()
{
    PrimaryActorTick.bCanEverTick = true;

    capsule_ = CreateDefaultSubobject<UCapsuleComponent>(TEXT("capsule"));
    spring_arm_ =
        CreateDefaultSubobject<USpringArmComponent>(TEXT("spring_arm"));
    camera_ = CreateDefaultSubobject<UCameraComponent>(TEXT("camera"));
    movement_ = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("movement"));

    RootComponent = capsule_;

    spring_arm_->SetupAttachment(RootComponent);
    camera_->SetupAttachment(spring_arm_, USpringArmComponent::SocketName);
}

void Acontroller_pawn::Tick(float dt)
{
    check(0.0f < camera_arm_move_speed);
    float const dist = target_target_arm_length_ - spring_arm_->TargetArmLength;
    if (dist * dist < 0.1) {
        spring_arm_->TargetArmLength = target_target_arm_length_;
    } else {
        spring_arm_->TargetArmLength +=
            dist / 2 * std::clamp(camera_arm_move_speed * dt, 0.0f, 2.0f);
    }
}

FVector Acontroller_pawn::camera_location() const
{
    FVector retval = GetActorLocation();
    retval.Z = -spring_arm_->TargetArmLength;
    return retval;
}

float Acontroller_pawn::target_arm_length() const
{
    return target_target_arm_length_;
}

void Acontroller_pawn::start_game_at(FVector location)
{
    spring_arm_->TargetArmLength = location.Z;
    target_target_arm_length_ = location.Z;
    location.Z = 0.0f;
    SetActorLocation(location);
}

void Acontroller_pawn::camera_location(
    FVector new_location, bool smooth_arm_motion)
{
    target_arm_length(-new_location.Z, smooth_arm_motion);
    new_location.Z = 0.0f;
    SetActorLocation(new_location);
}

void Acontroller_pawn::target_arm_length(float new_length, bool smooth_motion)
{
    target_target_arm_length_ = new_length;
    if (!smooth_motion)
        spring_arm_->TargetArmLength = new_length;
}
