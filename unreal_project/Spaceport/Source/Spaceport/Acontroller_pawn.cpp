#include "Acontroller_pawn.h"
#include "constants.hpp"
#include "map_transition.hpp"
#include "utility.hpp"

#include <algorithm>

#include <EnhancedInputComponent.h>
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
            if (map_transition_->in_transition())
                return;

            FVector2D const delta =
                value.Get<FVector2D>() * ui_defaults().camera_pan_speed_;
            AddMovementInput(FVector::UnitX(), delta.X);
            AddMovementInput(FVector::UnitY(), delta.Y);
        });

    eic->BindActionValueLambda(
        zoom_action_, ETriggerEvent::Triggered, [this](auto const & value) {
            if (map_transition_->in_transition())
                return;

            float const delta =
                value.Get<float>() * (ui_defaults().camera_zoom_speed_ +
                                      std::log(spring_arm_->TargetArmLength));
            UE_LOG(
                LogTemp,
                Log,
                TEXT("BEFORE: spring_arm_->TargetArmLength=%f delta=%f min=%f "
                     "max=%f"),
                spring_arm_->TargetArmLength,
                delta,
                min_camera_dist_for(map_mode_),
                max_camera_dist_for(map_mode_));
            spring_arm_->TargetArmLength = std::clamp(
                spring_arm_->TargetArmLength + delta,
                min_camera_dist_for(map_mode_),
                max_camera_dist_for(map_mode_));
            UE_LOG(
                LogTemp,
                Log,
                TEXT("AFTER: spring_arm_->TargetArmLength=%f delta=%f min=%f "
                     "max=%f"),
                spring_arm_->TargetArmLength,
                delta,
                min_camera_dist_for(map_mode_),
                max_camera_dist_for(map_mode_));
            if (map_mode_ == map_mode::system_map &&
                -just_inside_system_map < spring_arm_->TargetArmLength) {
                spring_arm_->TargetArmLength = -just_inside_system_map;
                if (map_transition_)
                    map_transition_->to_galaxy_map(camera_location());
            }
        });
}

FVector Acontroller_pawn::camera_location() const
{
    FVector retval = GetActorLocation();
    retval.Z = -spring_arm_->TargetArmLength;
    return retval;
}

void Acontroller_pawn::start_game_at(
    FVector location, std::shared_ptr<map_transition_state> map_transition)
{
    spring_arm_->TargetArmLength = location.Z;
    location.Z = 0.0f;
    SetActorLocation(location);
    map_mode_ = map_mode::galaxy_map;
    map_transition_ = std::move(map_transition);
}

void Acontroller_pawn::camera_location(FVector new_location)
{
    spring_arm_->TargetArmLength = -new_location.Z;
    new_location.Z = 0.0f;
    SetActorLocation(new_location);
}

void Acontroller_pawn::map_mode_changed(map_mode new_mode)
{
    map_mode_ = new_mode;
}
