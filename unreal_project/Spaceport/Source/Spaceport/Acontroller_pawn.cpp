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

void Acontroller_pawn::BeginPlay() { Super::BeginPlay(); }

void Acontroller_pawn::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void Acontroller_pawn::SetupPlayerInputComponent(UInputComponent * input)
{
    Super::SetupPlayerInputComponent(input);

    auto * eic = Cast<UEnhancedInputComponent>(input);
    if (!eic)
        return;
    eic->BindAction(
        slide_action_, ETriggerEvent::Triggered, this, &Acontroller_pawn::move);
    eic->BindAction(
        zoom_action_, ETriggerEvent::Triggered, this, &Acontroller_pawn::zoom);
}

void Acontroller_pawn::move(FInputActionValue const & value)
{
    FVector2D const delta = value.Get<FVector2D>();
    AddMovementInput(FVector::UnitX(), delta.X);
    AddMovementInput(FVector::UnitY(), delta.Y);
}

void Acontroller_pawn::zoom(FInputActionValue const & value)
{
    float const delta = value.Get<float>();
    spring_arm_->TargetArmLength = std::clamp(
        spring_arm_->TargetArmLength + delta, min_camera_dist, max_camera_dist);
}
