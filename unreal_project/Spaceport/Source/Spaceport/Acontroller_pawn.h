#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Acontroller_pawn.generated.h"


struct FInputActionValue;
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
