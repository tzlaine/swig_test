#include "Acontroller_pawn.h"
#include "constants.hpp"
#include "ui_defaults.h"
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
    SetActorTickEnabled(false);

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

    // TODO: Put the map size in Agame_state, and use that here to bound how
    // far we can move in any direction.

    auto * eic = Cast<UEnhancedInputComponent>(input);
    check(eic);

    if (!slide_action_ || !zoom_action_) {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("The controller pawn does not have all its actions assigned"));
        return;
    }

    eic->BindActionValueLambda(
        slide_action_, ETriggerEvent::Triggered, [this](auto const & value) {
            if (in_transition_)
                return;

            FVector2D const delta =
                value.Get<FVector2D>() * ui_defaults().camera_pan_speed_;
            AddMovementInput(FVector::UnitX(), delta.X);
            AddMovementInput(FVector::UnitY(), delta.Y);
        });

    eic->BindActionValueLambda(
        zoom_action_, ETriggerEvent::Triggered, [this](auto const & value) {
            if (in_transition_)
                return;

            float const delta =
                value.Get<float>() * (ui_defaults().camera_zoom_speed_ +
                                      std::log(spring_arm_->TargetArmLength));
            spring_arm_->TargetArmLength = std::clamp(
                spring_arm_->TargetArmLength + delta,
                min_camera_dist,
                max_camera_dist);
        });

    GetWorldTimerManager().SetTimerForNextTick([this] {
        spring_arm_->TargetArmLength -= map_actors_vertical_offset;
    });
}

void Acontroller_pawn::Tick(float delta)
{
    Super::Tick(delta);

    float const close_enough = 0.001f;
    if (!in_transition_ ||
        system_view_transition_time_s - close_enough < transition_progress_) {
        SetActorTickEnabled(false);
        if (in_transition_ && transition_done_cb_)
            transition_done_cb_();
        in_transition_ = false;
        return;
    }

    float const smooth_alpha = FMath::SmoothStep(
        0.0f,
        1.0f,
        std::min(transition_progress_ / system_view_transition_time_s, 1.0f));
    FVector new_location = FMath::Lerp(
        initial_camera_location_, desired_camera_location_, smooth_alpha);
    new_location.Z = FMath::Lerp(
        initial_camera_location_.Z,
        desired_camera_location_.Z,
        FMath::Pow(smooth_alpha, 3));
    spring_arm_->TargetArmLength = -new_location.Z;
    FVector const camera_location = new_location;
    new_location.Z = 0.0f;
    SetActorLocation(new_location);
    transition_progress_ += delta;
}

void Acontroller_pawn::system_view_transition(
    FVector system_location, std::function<void()> done_cb)
{
    in_transition_ = true;
    transition_progress_ = 0.0f;
    initial_camera_location_ = GetActorLocation();
    initial_camera_location_.Z = -spring_arm_->TargetArmLength;
    desired_camera_location_ = system_location;
    desired_camera_location_.Z = map_actors_vertical_offset + 50;
    transition_done_cb_ = std::move(done_cb);
    SetActorTickEnabled(true);

    FVector system_star_location{};
    FVector map_system_location = desired_camera_location_;
    map_system_location.Z = map_actors_vertical_offset;
    system_star_location = FMath::RayPlaneIntersection(
        initial_camera_location_,
        map_system_location - initial_camera_location_,
        FPlane(
            FVector(
                0, 0, desired_camera_location_.Z - initial_camera_location_.Z),
            FVector(0, 0, -1)));
}
