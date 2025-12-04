#pragma once

#include <CoreMinimal.h>
#include <GameFramework/Pawn.h>
#include "Amap_fleet_pawn.generated.h"


class UCapsuleComponent;
class UFloatingPawnMovement;
class UStaticMeshComponent;

UCLASS()
class Amap_fleet_pawn : public APawn
{
    GENERATED_BODY()

public:
    Amap_fleet_pawn();

    void BeginPlay() override;
    void Tick(float delta) override;

    void select(bool b);

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
        Category = "Movement",
        meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UFloatingPawnMovement> movement_;

    UPROPERTY(
        VisibleAnywhere,
        BlueprintReadOnly,
        Category = "Collision",
        meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UStaticMeshComponent> mesh_;

    UPROPERTY(
        VisibleAnywhere,
        BlueprintReadOnly,
        Category = "Collision",
        meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UStaticMeshComponent> selection_indicator_;

};
