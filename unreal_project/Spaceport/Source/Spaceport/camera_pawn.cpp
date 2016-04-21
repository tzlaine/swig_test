// Fill out your copyright notice in the Description page of Project Settings.

#include "Spaceport.h"
#include "camera_pawn.h"


namespace {

    bool mouse_moved (float x, float y, float prev_x, float prev_y)
    {
        return fabs(x - prev_x) > 0.1 || fabs(y - prev_y) > 0.1;
    }

}


Acamera_pawn::Acamera_pawn () :
    rotation_speed_ (5.0f),
    mouse_button_down_ {false, false, false},
    drag_ (drag::none)
{
    // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    AutoReceiveInput = EAutoReceiveInput::Player0;

    z_axis_gimbal_ = CreateDefaultSubobject<USphereComponent>(TEXT("z_axis_gimbal"));
    z_axis_gimbal_->AttachTo(RootComponent);

    x_axis_gimbal_ = CreateDefaultSubobject<USphereComponent>(TEXT("x_axis_gimbal"));
    x_axis_gimbal_->AttachTo(z_axis_gimbal_);
    x_axis_gimbal_->SetWorldRotation(FRotator(105.0f, -90.0f, 0.0f));

    spring_arm_ = CreateDefaultSubobject<USpringArmComponent>(TEXT("spring_arm"));
    spring_arm_->AttachTo(x_axis_gimbal_);
    spring_arm_->TargetArmLength = 300.0f;

    camera_ = CreateDefaultSubobject<UCameraComponent>(TEXT("camera"));
    camera_->AttachTo(spring_arm_);
}

void Acamera_pawn::BeginPlay ()
{
    Super::BeginPlay();
    player_controller_ = Cast<APlayerController>(GetController());
    player_controller_->bShowMouseCursor = true;
}

void Acamera_pawn::Tick (float DeltaTime)
{
    Super::Tick(DeltaTime);
    check_for_drag();
    drag_update();
}

void Acamera_pawn::SetupPlayerInputComponent (class UInputComponent* InputComponent)
{
    Super::SetupPlayerInputComponent(InputComponent);
}

void Acamera_pawn::drag_update ()
{
    float x;
    float y;
    player_controller_->GetMousePosition(x, y);
    if (!mouse_moved(x, y, prev_mouse_drag_x_, prev_mouse_drag_y_))
        return;

    float const delta_x = x - prev_mouse_drag_x_;
    float const delta_y = y - prev_mouse_drag_y_;

    switch (drag_) {
    case drag::none: return;

    case drag::rotation: {
        z_axis_gimbal_->AddLocalRotation(FRotator(0, delta_x * -rotation_speed_, 0));

        float const rotation = delta_y * rotation_speed_;
        FRotator rotator = x_axis_gimbal_->GetComponentRotation();
        rotator.Pitch = FMath::Clamp(rotator.Pitch + rotation, -15.0f, 75.0f);
        x_axis_gimbal_->SetWorldRotation(rotator);

        prev_mouse_drag_x_ = x;
        prev_mouse_drag_y_ = y;
        break;
    }

    case drag::map_drag: {
//            GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::White, TEXT("dragging map..."));

        prev_mouse_drag_x_ = x;
        prev_mouse_drag_y_ = y;
        break;
    }
    }
}
