#pragma once

#include <functional>

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
    void Tick(float delta);

    void system_view_transition(
        FVector system_location, std::function<void()> done_cb);

private:
    bool in_transition_ = false;
    float transition_progress_ = 0.0f;
    FVector initial_camera_location_ = FVector();
    FVector desired_camera_location_ = FVector();
    std::function<void()> transition_done_cb_;

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
