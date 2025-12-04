#include "Acontroller_pawn.h"
#include "constants.hpp"
#include "utility.hpp"

#include <algorithm>

#include <EnhancedInputComponent.h>
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

void Acontroller_pawn::SetupPlayerInputComponent(UInputComponent * input)
{
    Super::SetupPlayerInputComponent(input);

    auto * eic = Cast<UEnhancedInputComponent>(input);
    check(eic);
    eic->BindActionValueLambda(
        slide_action_, ETriggerEvent::Completed, [this](auto const & value) {
            FVector2D const delta = value.Get<FVector2D>();
            AddMovementInput(FVector::UnitX(), delta.X);
            AddMovementInput(FVector::UnitY(), delta.Y);
        });
    eic->BindActionValueLambda(
        zoom_action_, ETriggerEvent::Completed, [this](auto const & value) {
            float const delta = value.Get<float>();
            spring_arm_->TargetArmLength = std::clamp(
                spring_arm_->TargetArmLength + delta,
                min_camera_dist,
                max_camera_dist);
        });
}
