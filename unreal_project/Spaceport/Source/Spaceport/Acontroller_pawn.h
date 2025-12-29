#pragma once

#include <memory>

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Acontroller_pawn.generated.h"


class UCameraComponent;
class UCapsuleComponent;
class UFloatingPawnMovement;
class USpringArmComponent;

UCLASS()
class Acontroller_pawn : public APawn
{
    GENERATED_BODY()

public:
    Acontroller_pawn();

    FVector camera_location() const;
    float target_arm_length() const;

    void start_game_at(FVector location);
    void camera_location(FVector new_location);
    void target_arm_length(float new_length);

private:
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
        VisibleAnywhere,
        BlueprintReadOnly,
        Category = "Movement",
        meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UFloatingPawnMovement> movement_;
};
