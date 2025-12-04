#pragma once

#include <CoreMinimal.h>
#include <GameFramework/Actor.h>
#include "Amap_hex.generated.h"


class UCapsuleComponent;
class UStaticMeshComponent;

UCLASS()
class Amap_hex : public AActor
{
    GENERATED_BODY()

public:
    Amap_hex();

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
