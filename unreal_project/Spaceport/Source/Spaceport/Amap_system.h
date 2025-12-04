#pragma once

#include "Amap_pawn_base.h"

#include <CoreMinimal.h>
#include "Amap_system.generated.h"


class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class Amap_system : public Amap_pawn_base
{
    GENERATED_BODY()

public:
    Amap_system();

    void BeginPlay() override;
    void Tick(float delta) override;

    void select(bool b) override;
    void hover(bool b) override;
    map_pawn_kind kind() const override { return map_pawn_kind::system; }

    UStaticMeshComponent * static_mesh() { return mesh_; }

private:
    UPROPERTY(
        VisibleAnywhere,
        BlueprintReadOnly,
        Category = "Components",
        meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USphereComponent> sphere_;

    UPROPERTY(
        VisibleAnywhere,
        BlueprintReadOnly,
        Category = "Components",
        meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UStaticMeshComponent> mesh_;

    UPROPERTY(
        VisibleAnywhere,
        BlueprintReadOnly,
        Category = "Components",
        meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UStaticMeshComponent> selection_indicator_;
};
