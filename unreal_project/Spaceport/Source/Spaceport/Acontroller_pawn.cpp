#include "Acontroller_pawn.h"

#include <Camera/CameraComponent.h>
#include <Components/CapsuleComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <GameFramework/FloatingPawnMovement.h>


Acontroller_pawn::Acontroller_pawn()
{
    PrimaryActorTick.bCanEverTick = false;

    capsule_ = CreateDefaultSubobject<UCapsuleComponent>(TEXT("capsule"));
    spring_arm_ =
        CreateDefaultSubobject<USpringArmComponent>(TEXT("spring_arm"));
    camera_ = CreateDefaultSubobject<UCameraComponent>(TEXT("camera"));
    movement_ = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("movement"));

    RootComponent = capsule_;

    spring_arm_->SetupAttachment(RootComponent);
    camera_->SetupAttachment(spring_arm_, USpringArmComponent::SocketName);
}

FVector Acontroller_pawn::camera_location() const
{
    FVector retval = GetActorLocation();
    retval.Z = -spring_arm_->TargetArmLength;
    return retval;
}

float Acontroller_pawn::target_arm_length() const
{
    return spring_arm_->TargetArmLength;
}

void Acontroller_pawn::start_game_at(FVector location)
{
    spring_arm_->TargetArmLength = location.Z;
    location.Z = 0.0f;
    SetActorLocation(location);
}

void Acontroller_pawn::camera_location(FVector new_location)
{
    spring_arm_->TargetArmLength = -new_location.Z;
    new_location.Z = 0.0f;
    SetActorLocation(new_location);
}

void Acontroller_pawn::target_arm_length(float new_length)
{
    spring_arm_->TargetArmLength = new_length;
}
