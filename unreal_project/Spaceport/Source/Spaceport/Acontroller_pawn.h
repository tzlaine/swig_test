#pragma once

#include "map_transition_fwd.hpp"

#include <memory>

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Acontroller_pawn.generated.h"


class UCameraComponent;
class UCapsuleComponent;
class UFloatingPawnMovement;
class UInputAction;
class UInputComponent;
class USpringArmComponent;

UCLASS()
class Acontroller_pawn : public APawn
{
    GENERATED_BODY()

public:
    Acontroller_pawn();

    void SetupPlayerInputComponent(UInputComponent * input) override;

    FVector camera_location() const;

    void start_game_at(
        FVector location, std::shared_ptr<map_transition_state> map_transition);
    void camera_location(FVector new_location);
    void map_mode_changed(map_mode new_mode);

private:
    std::shared_ptr<map_transition_state> map_transition_;
    map_mode map_mode_ = map_mode::galaxy_map;

    UPROPERTY(
        VisibleAnywhere,
        BlueprintReadOnly,
        Category = "Collision",
        meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UCapsuleComponent> capsule_;

    UPROPERTY(
        VisibleAnywhere,
        BlueprintReadOnly,
        Category = "Camera",
        meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USpringArmComponent> spring_arm_;

    UPROPERTY(
        VisibleAnywhere,
        BlueprintReadOnly,
        Category = "Camera",
        meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UCameraComponent> camera_;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Input",
        meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> slide_action_;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Input",
        meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> zoom_action_;

    UPROPERTY(
        VisibleAnywhere,
        BlueprintReadOnly,
        Category = "Movement",
        meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UFloatingPawnMovement> movement_;
};
